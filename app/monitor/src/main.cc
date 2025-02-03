#include <Arduino.h>
#include <M5Stack.h>
#include <freertos/FreeRTOS.h>
#include <freertos/stream_buffer.h>
#include <lvgl.h>
#include <okdcc.h>

// 最後に include する
#include <M5GFX.h>

#if 8 != CHAR_BIT
#error "This program expects that the bit size of \"char\" is 8."
#endif

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))
#define SIGNAL_BUFFER_SIZE 256
#define LOG_STREAM_BUFFER_SIZE 512
#define VOLTAGE_GPIO 5
#define STACK_SIZE 4096

#define LOG(...)                                      \
  do {                                                \
    printLog("LOG", __FILE__, __LINE__, __func__, __VA_ARGS__); \
  } while (0)

extern "C" {
void displayFlush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
void readButtons(lv_indev_t *indev, lv_indev_data_t *data);
void onVoltageChange(void);
[[noreturn]] void errorLoop(void);
int printDebugLog(char const *const file, int const line, char const *func, char const *format, ...);
void printErrorLog(char const *const file, int const line, char const *func, char const *format, ...);
void printLog(char const *const level, char const *const file, int const line, char const *func, char const *format, ...);
void printLogVa(char const *const level, char const *const file, int const line, char const *func, char const *format, va_list vlist);
void printLogTask(void *parameters);
}

#if LV_USE_LOG
void printLvLog(lv_log_level_t level, const char *buf);
#endif

static M5GFX gfx;
static dcc_TimeMicroSec signalBufferValues[SIGNAL_BUFFER_SIZE];
static struct dcc_Decoder decoder = dcc_initializeDecoder(signalBufferValues, SIGNAL_BUFFER_SIZE);
static StreamBufferHandle_t logStreamBuffer = NULL;
static char logStreamBufferStorage[LOG_STREAM_BUFFER_SIZE + 1] = { 0 }; // StreamBuffer が 1 バイト余分に要求する
static StaticStreamBuffer_t logStreamBufferStruct;

void setup() {
  M5.begin();
  gfx.init();
  lv_init();
  Serial.begin(115200);
  Serial.printf("OKDCC v0.0 (%s %s)\n", __DATE__, __TIME__);
  Serial.printf("LVGL v%d.%d.%d\n", lv_version_major(), lv_version_minor(), lv_version_patch());

  lv_init();

#if LV_USE_LOG
  lv_log_register_print_cb(printLvLog);
#endif

  lv_tick_set_cb((lv_tick_get_cb_t) millis);

  lv_display_t *display = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  static uint8_t draw_buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 10 * BYTE_PER_PIXEL];
  lv_display_set_buffers(display, draw_buffer, NULL, sizeof(draw_buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(display, displayFlush);

  lv_indev_t *buttonsIndev = lv_indev_create();
  lv_indev_set_type(buttonsIndev, LV_INDEV_TYPE_KEYPAD);
  lv_indev_set_read_cb(buttonsIndev, readButtons);

  logStreamBuffer =
    xStreamBufferCreateStatic(LOG_STREAM_BUFFER_SIZE, 1, (uint8_t *)logStreamBufferStorage, &logStreamBufferStruct);
  dcc_error_log = printErrorLog;

  struct dcc_ui_Model_Command modelCommand = dcc_ui_init(buttonsIndev);
  dcc_ui_view(modelCommand.model);

  pinMode(VOLTAGE_GPIO, INPUT_PULLDOWN);
  {
    int pin = digitalPinToInterrupt(VOLTAGE_GPIO);
    if (NOT_AN_INTERRUPT == pin) {
      LOG("Failed to set GPIO %d as interrupt", VOLTAGE_GPIO);
      errorLoop();
    }
    attachInterrupt(pin, onVoltageChange, CHANGE);
  }

  TaskHandle_t printLogTaskHandle = NULL;
  BaseType_t printLogTaskCreationResult =
    xTaskCreate(printLogTask, "printLogTask", STACK_SIZE, NULL, 2, &printLogTaskHandle);
  if (pdPASS != printLogTaskCreationResult) {
    LOG("Failed to create \"printLogTask\" task");
    errorLoop();
  }
}

void loop() {
  M5.update();
  lv_timer_handler();
  {
    dcc_TimeMicroSec signal;
    char buffer[512] = { 0 };
    dcc_showSignalBuffer(buffer, sizeof buffer, decoder.signalBuffer);
    while (dcc_Success == dcc_readSignalBuffer(&decoder.signalBuffer, &signal)) {
      dcc_showSignalBuffer(buffer, sizeof buffer, decoder.signalBuffer);
      struct dcc_Packet packet;
      enum dcc_StreamParserResult result = dcc_decode(&decoder, signal, &packet);
      switch (result) {
        case dcc_StreamParserResult_Failure:
          LOG("decode error");
          continue;
        case dcc_StreamParserResult_Continue:
          LOG("decode continue");
          continue;
        case dcc_StreamParserResult_Success: {
          char buffer[512] = { 0 };
          dcc_showPacket(buffer, sizeof buffer, packet);
          LOG("packet: %s", buffer);
          continue;
        }
      }
    }
  }
  delay(1000 / 60);
}

void displayFlush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map) {
  unsigned int const w = area->x2 - area->x1 + 1;
  unsigned int const h = area->y2 - area->y1 + 1;

  gfx.startWrite();
  gfx.setAddrWindow(area->x1, area->y1, w, h);
  gfx.writePixels((uint16_t *) px_map, w * h, true);
  gfx.endWrite();

  lv_display_flush_ready(display);
}

void readButtons(lv_indev_t *indev, lv_indev_data_t *data) {
  static int index = 0;
  static Button buttons[] = { M5.BtnA, M5.BtnB, M5.BtnC };
  static uint32_t const keys[] = { LV_KEY_NEXT, LV_KEY_PREV, LV_KEY_ENTER };

  buttons[index].read();
  if (buttons[index].wasPressed()) {
    data->state = LV_INDEV_STATE_PRESSED;
    data->key = keys[index];
  } else if (buttons[index].wasReleased()) {
    data->state = LV_INDEV_STATE_RELEASED;
    data->key = keys[index];
  }

  index = (index + 1) % 3;
}

enum dcc_Result readSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signal) {
  noInterrupts();
  enum dcc_Result result = dcc_readSignalBuffer(buffer, signal);
  interrupts();
  return result;
}

void onVoltageChange() {
  static unsigned long last = 0;
  unsigned long const now = micros();
  if (now - last < 10) return;
  last = now;
  if (dcc_Failure == dcc_writeSignalBuffer(&decoder.signalBuffer, micros()))
    LOG("Failed to write signal buffer");
}

void errorLoop(void) {
  while (true) {
    delay(1000);
  }
}

void printLogTask(void *parameters) {
  char buffer[LOG_STREAM_BUFFER_SIZE + 1] = { 0 }; // 終端ヌル文字の1文字分を余分に確保する
  while (true) {
    if (logStreamBuffer == NULL) {
      vTaskDelay(pdMS_TO_TICKS(1000 / 60));
      continue;
    }
    size_t const read = xStreamBufferReceive(logStreamBuffer, buffer, LOG_STREAM_BUFFER_SIZE, portMAX_DELAY);
    Serial.printf("printLogTask: read: %u\n", read);
    buffer[read] = '\0';
    Serial.print(buffer);
    vTaskDelay(pdMS_TO_TICKS(1000 / 60));
  }
  vTaskDelete(NULL);
}

#if LV_USE_LOG
void printLvLog(lv_log_level_t level, const char *buf) {
  static const char *lvl_prefix[] = { "TRACE", "INFO", "WARN", "ERROR", "USER" };
  Serial.printf("LVGL: %s: %s\n", lvl_prefix[level], buf);
}
#endif

int printDebugLog(char const *const file, int const line, char const *func, char const *format, ...) {
  va_list vlist;
  va_start(vlist, format);
  printLogVa("DEBUG", file, line, func, format, vlist);
  va_end(vlist);
  return 0;
}

void printErrorLog(char const *const file, int const line, char const *func, char const *format, ...) {
  va_list vlist;
  va_start(vlist, format);
  printLogVa("ERROR", file, line, func, format, vlist);
  va_end(vlist);
  exit(EXIT_FAILURE);
}

void printLog(char const *const level, char const *const file, int const line, char const *func, char const *format, ...) {
  va_list vlist;
  va_start(vlist, format);
  printLogVa(level, file, line, func, format, vlist);
  va_end(vlist);
}

void printLogVa(char const *const level, char const *const file, int const line, char const *func, char const *format, va_list vlist) {
  size_t const lineBufferSize = 216;
  char lineBuffer[lineBufferSize] = { 0 };
  {
    size_t const messageBufferSize = 216;
    char messageBuffer[messageBufferSize] = { 0 };
    {
      int const result = vsnprintf(messageBuffer, messageBufferSize - 1, format, vlist); // 終端ヌル文字用に1文字開けておく
      if (result < 0) return; // 書き込みに失敗した
      // messageBufferSize ≦ result のときはバッファに書き込み切れなかった
      // 書き込まれたうしろにはヌル文字があるはず
      assert(messageBuffer[result] == '\0');
    }
    {
      int const result = snprintf(lineBuffer, lineBufferSize - 1, "%s: %s (%d) %s: %s\n", level, file, line, func, messageBuffer); // 終端ヌル文字用に1文字開けておく
      if (result < 0) return; // 書き込みに失敗した
      // lineBufferSize ≦ result のときはバッファに書き込み切れなかった
      assert(lineBuffer[result] == '\0');
    }
  }
  if (xPortInIsrContext()) {
    if (logStreamBuffer == NULL) return;
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xStreamBufferSendFromISR(logStreamBuffer, lineBuffer, strlen(lineBuffer), &higherPriorityTaskWoken); // 終端ヌル文字は送信しない
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
  } else
    Serial.print(lineBuffer);
  return;
}
