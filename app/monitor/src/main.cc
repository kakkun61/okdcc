#include <Arduino.h>
#include <M5Stack.h>
#include <lvgl.h>
#include <okdcc.h>

// 最後に include する
#include <M5GFX.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))

void displayFlush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
void readButtons(lv_indev_t *indev, lv_indev_data_t *data);

#if LV_USE_LOG
void printLog(lv_log_level_t level, const char *buf);
#endif

static M5GFX gfx;

void setup() {
  M5.begin();
  gfx.init();
  lv_init();
  Serial.begin(115200);

  Serial.printf("LVGL v%d.%d.%d\n", lv_version_major(), lv_version_minor(), lv_version_patch());

  lv_init();

#if LV_USE_LOG
  lv_log_register_print_cb(printLog);
#endif

  lv_tick_set_cb((lv_tick_get_cb_t) millis);

  lv_display_t *display = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  static uint8_t draw_buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 10 * BYTE_PER_PIXEL];
  lv_display_set_buffers(display, draw_buffer, NULL, sizeof(draw_buffer),
                              LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(display, displayFlush);

  lv_indev_t *buttonsIndev = lv_indev_create();
  lv_indev_set_type(buttonsIndev, LV_INDEV_TYPE_KEYPAD);
  lv_indev_set_read_cb(buttonsIndev, readButtons);

  struct dcc_ui_Model_Command modelCommand = dcc_ui_init(buttonsIndev);
  dcc_ui_view(modelCommand.model);
}

void loop() {
  M5.update();
  lv_timer_handler();
  delay(5);
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

#if LV_USE_LOG
void printLog(lv_log_level_t level, const char *buf) {
  static const char *lvl_prefix[] = { "Trace", "Info", "Warn", "Error", "User" };
  Serial.printf("%s: %s\n", lvl_prefix[level], buf);
}
#endif
