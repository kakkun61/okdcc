#include <Arduino.h>
#include <M5Stack.h>
#include <lvgl.h>

// 最後に include する
#include <M5GFX.h>

static uint16_t const screenWidth = 320;
static uint16_t const screenHeight = 240;

static M5GFX gfx;

void setup() {
  M5.begin();
  gfx.init();
  lv_init();
  Serial.begin(115200);

  Serial.printf("LVGL v%d.%d.%d\n", lv_version_major(), lv_version_minor(), lv_version_patch());

  lv_init();

  lv_display_t *display = lv_display_create(screenWidth, screenHeight);
  static uint8_t draw_buffer[screenWidth * screenHeight / 10];
  lv_display_set_draw_buffers(display, draw_buffer, NULL, screenWidth * screenHeight / 10, LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(display, display_flush);

  lv_example_btn_1();
}

void loop() {
  M5.update();
  lv_timer_handler();
  delay(5);
}

void display_flush(lv_display_t *display, const lv_area_t *area, uint8_t *px_map) {
  unsigned int const w = area->x2 - area->x1 + 1;
  unsigned int const h = area->y2 - area->y1 + 1;

  gfx.startWrite();
  gfx.setAddrWindow(area->x1, area->y1, w, h);
  gfx.writePixels((uint16_t *) px_map, w * h, true);
  gfx.endWrite();

  lv_display_flush_ready(display);
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");
  } else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
  }
}

void lv_example_btn_1(void) {
  lv_obj_t *label;

  lv_obj_t *btn1 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);

  label = lv_label_create(btn1);
  lv_label_set_text(label, "Button");
  lv_obj_center(label);

  lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
  lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 40);
  lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
  lv_obj_set_height(btn2, LV_SIZE_CONTENT);

  label = lv_label_create(btn2);
  lv_label_set_text(label, "Toggle");
  lv_obj_center(label);
}
