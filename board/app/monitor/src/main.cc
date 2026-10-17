#define LGFX_AUTODETECT
#include <LGFX_AUTODETECT.hpp>
#include <LovyanGFX.hpp>

#define CLAY_IMPLEMENTATION
#include <clay.h>
#include <freertos/FreeRTOS.h>
#include <freertos/stream_buffer.h>
#include <okdcc.h>

#include <ClayLGFX.hpp>

#include "monitor_ui.hpp"

#if 8 != CHAR_BIT
#error "This program expects that the bit size of \"char\" is 8."
#endif

#define SIGNAL_BUFFER_SIZE 1024
#define LOG_STREAM_BUFFER_SIZE (4 * 1024)
// フォトカプラーの出力を受け取るピン
// レールの極性が反転するたびにレベルが変わる
// フォトカプラーの出力はオープンコレクターを想定してプルアップする
#define DCC_SIGNAL_GPIO GPIO_NUM_5
#define BUTTON_A_GPIO 39
#define BUTTON_B_GPIO 38
#define BUTTON_C_GPIO 37
#define DISPLAY_BAND_HEIGHT 40
#define CLAY_MAX_ELEMENT_COUNT 128

#define LOG(...)                                                \
  do {                                                          \
    printLog("LOG", __FILE__, __LINE__, __func__, __VA_ARGS__); \
  } while (0)

extern "C" {
void app_main(void);
void loopTask(void *);
void onVoltageChange(void *);
enum dcc_Result readSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signal);
[[noreturn]] void errorLoop(void);
int printDebugLog(char const *const file, int const line, char const *func, char const *format, ...);
void printErrorLog(char const *const file, int const line, char const *func, char const *format, ...);
void printLog(char const *const level, char const *const file, int const line, char const *func, char const *format,
              ...);
void printLogVa(char const *const level, char const *const file, int const line, char const *func, char const *format,
                va_list vlist);
void printLogTask(void *);
}

static void printClayError(Clay_ErrorData error);

static LGFX display;
static clay_lgfx::FontTable fontTable;
static clay_lgfx::Renderer renderer(fontTable);
static clay_lgfx::BandedPresenter presenter(display, renderer);
static clay_lgfx::GpioButtons buttons({ BUTTON_A_GPIO, BUTTON_B_GPIO, BUTTON_C_GPIO });
static char uiFrameMemory[monitor_ui::frameMemorySize];
static okdcc::ui::Frame uiFrame(uiFrameMemory, sizeof uiFrameMemory);
static monitor_ui::Program uiProgram(monitor_ui::definition, uiFrame);
static dcc_TimeMicroSec signalBufferValues[SIGNAL_BUFFER_SIZE];
static struct dcc_Decoder decoder = dcc_initializeDecoder(signalBufferValues, SIGNAL_BUFFER_SIZE);
static StreamBufferHandle_t logStreamBuffer = NULL;
static char logStreamBufferStorage[LOG_STREAM_BUFFER_SIZE + 1] = { 0 };  // StreamBuffer が 1 バイト余分に要求する
static StaticStreamBuffer_t logStreamBufferStruct;
static portMUX_TYPE signalBufferMux = portMUX_INITIALIZER_UNLOCKED;

void app_main(void) {
  printf("OKDCC v0.0 (%s %s)\n", __DATE__, __TIME__);

  logStreamBuffer =
    xStreamBufferCreateStatic(LOG_STREAM_BUFFER_SIZE, 1, (uint8_t *) logStreamBufferStorage, &logStreamBufferStruct);
  dcc_error_log = printErrorLog;

  display.init();
  fontTable.set(0, &lgfx::fonts::Font0);
  fontTable.set(1, &lgfx::fonts::Font2);
  Clay_SetMaxElementCount(CLAY_MAX_ELEMENT_COUNT);
  Clay_SetMaxMeasureTextCacheWordCount(512);
  {
    uint32_t const size = Clay_MinMemorySize();
    void *const memory = malloc(size);
    if (memory == NULL) {
      LOG("Failed to allocate %lu bytes for Clay", (unsigned long) size);
      errorLoop();
    }
    Clay_Initialize(Clay_CreateArenaWithCapacityAndMemory(size, memory),
                    Clay_Dimensions{ static_cast<float>(display.width()), static_cast<float>(display.height()) },
                    Clay_ErrorHandler{ printClayError, nullptr });
  }
  fontTable.attach();
  if (!presenter.begin(DISPLAY_BAND_HEIGHT)) {
    LOG("Failed to allocate the display band buffer");
    errorLoop();
  }
  buttons.begin();
  uiProgram.start();

  {
    gpio_config_t config = { .pin_bit_mask = BIT64(DCC_SIGNAL_GPIO),
                             .mode = GPIO_MODE_INPUT,
                             .pull_up_en = GPIO_PULLUP_ENABLE,
                             .pull_down_en = GPIO_PULLDOWN_DISABLE,
                             .intr_type = GPIO_INTR_ANYEDGE };
    esp_err_t const result = gpio_config(&config);
    if (ESP_OK != result) {
      LOG("Failed to configure GPIO %d: %s", DCC_SIGNAL_GPIO, esp_err_to_name(result));
      errorLoop();
    }
  }
  {
    esp_err_t const result = gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    if (ESP_OK != result) {
      LOG("Failed to install ISR service: %s", esp_err_to_name(result));
      errorLoop();
    }
  }
  {
    esp_err_t const result = gpio_isr_handler_add(DCC_SIGNAL_GPIO, onVoltageChange, NULL);
    if (ESP_OK != result) {
      LOG("Failed to add ISR handler on GPIO %d: %s", DCC_SIGNAL_GPIO, esp_err_to_name(result));
      errorLoop();
    }
  }

  {
    TaskHandle_t printLogTaskHandle = NULL;
    BaseType_t printLogTaskCreationResult =
      xTaskCreate(printLogTask, "printLogTask", /* stack size */ 8 * 1096, NULL, 2, &printLogTaskHandle);
    if (pdPASS != printLogTaskCreationResult) {
      LOG("Failed to create \"printLogTask\" task");
      errorLoop();
    }
  }

  {
    TaskHandle_t loopTaskHandle = NULL;
    BaseType_t loopTaskCreationResult =
      xTaskCreate(loopTask, "loopTask", /* stack size */ 24 * 1024, NULL, 1, &loopTaskHandle);
    if (pdPASS != loopTaskCreationResult) {
      LOG("Failed to create \"loopTask\" task");
      errorLoop();
    }
  }
}

static void printClayError(Clay_ErrorData error) {
  LOG("Clay: %.*s", static_cast<int>(error.errorText.length), error.errorText.chars);
}

void loopTask(void *) {
  int64_t lastTime = esp_timer_get_time();
  while (true) {
    {
      dcc_TimeMicroSec signal;
      while (dcc_Success == readSignalBuffer(&decoder.signalBuffer, &signal)) {
        struct dcc_Packet packet;
        enum dcc_StreamParserResult result = dcc_decode(&decoder, signal, &packet);
        switch (result) {
          case dcc_StreamParserResult_Failure:
            LOG("decode error");
            uiProgram.dispatch({ .tag = monitor_ui::MessageTag::DecodeFailed, .packet = {} });
            continue;
          case dcc_StreamParserResult_Continue:
            // LOG("decode continue");
            continue;
          case dcc_StreamParserResult_Success: {
            char buffer[512] = { 0 };
            dcc_showPacket(buffer, sizeof buffer, packet);
            LOG("packet: %s", buffer);
            uiProgram.dispatch({ .tag = monitor_ui::MessageTag::PacketReceived, .packet = packet });
            continue;
          }
        }
      }
    }
    uiProgram.handleButtons(buttons.read());
    {
      int64_t const now = esp_timer_get_time();
      float const deltaTime = static_cast<float>(now - lastTime) / 1000000.0f;
      lastTime = now;
      presenter.present(uiProgram.frame(deltaTime));
    }
    vTaskDelay(1);
  }
  vTaskDelete(NULL);
}

enum dcc_Result readSignalBuffer(struct dcc_SignalBuffer *const buffer, dcc_TimeMicroSec *const signal) {
  portENTER_CRITICAL(&signalBufferMux);
  enum dcc_Result const result = dcc_readSignalBuffer(buffer, signal);
  portEXIT_CRITICAL(&signalBufferMux);
  return result;
}

// spell-checker:ignore IRAM
void IRAM_ATTR onVoltageChange(void *) {
  static int lastLevel = -1;
  int const level = gpio_get_level(DCC_SIGNAL_GPIO);
  if (level == lastLevel) return;
  lastLevel = level;
  unsigned long const now = esp_timer_get_time();
  portENTER_CRITICAL_ISR(&signalBufferMux);
  enum dcc_Result const result = dcc_writeSignalBuffer(&decoder.signalBuffer, now);
  portEXIT_CRITICAL_ISR(&signalBufferMux);
  if (dcc_Failure == result) LOG("Failed to write signal buffer");
}

void errorLoop(void) {
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void printLogTask(void *) {
  char buffer[LOG_STREAM_BUFFER_SIZE + 1] = { 0 };  // 終端ヌル文字の1文字分を余分に確保する
  while (true) {
    assert(logStreamBuffer != NULL);
    size_t const read = xStreamBufferReceive(logStreamBuffer, buffer, LOG_STREAM_BUFFER_SIZE, portMAX_DELAY);
    buffer[read] = '\0';
    printf(buffer);
  }
  vTaskDelete(NULL);
}

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

void printLog(char const *const level, char const *const file, int const line, char const *func, char const *format,
              ...) {
  va_list vlist;
  va_start(vlist, format);
  printLogVa(level, file, line, func, format, vlist);
  va_end(vlist);
}

void printLogVa(char const *const level, char const *const file, int const line, char const *func, char const *format,
                va_list vlist) {
  if (logStreamBuffer == NULL) return;
  size_t const lineBufferSize = 216;
  char lineBuffer[lineBufferSize] = { 0 };
  {
    size_t const messageBufferSize = 216;
    char messageBuffer[messageBufferSize] = { 0 };
    {
      int const result =
        vsnprintf(messageBuffer, messageBufferSize - 1, format, vlist);  // 終端ヌル文字用に1文字開けておく
      if (result < 0) return;                                            // 書き込みに失敗した
      // messageBufferSize ≦ result のときはバッファに書き込み切れなかった
      // 書き込まれたうしろにはヌル文字があるはず
      assert(messageBuffer[result] == '\0');
    }
    {
      int const result = snprintf(lineBuffer,
                                  lineBufferSize - 1,
                                  "%s: %s (%d) %s: %s\n",
                                  level,
                                  file,
                                  line,
                                  func,
                                  messageBuffer);  // 終端ヌル文字用に1文字開けておく
      if (result < 0) return;                      // 書き込みに失敗した
      // lineBufferSize ≦ result のときはバッファに書き込み切れなかった
      assert(lineBuffer[result] == '\0');
    }
  }
  if (xPortInIsrContext()) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xStreamBufferSendFromISR(logStreamBuffer,
                             lineBuffer,
                             strlen(lineBuffer),
                             &higherPriorityTaskWoken);  // 終端ヌル文字は送信しない
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
  } else {
    xStreamBufferSend(logStreamBuffer, lineBuffer, strlen(lineBuffer), 0);  // 終端ヌル文字は送信しない
    portYIELD();
  }
  return;
}
