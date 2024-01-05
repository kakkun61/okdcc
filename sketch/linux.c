#define _DEFAULT_SOURCE  // necessary for usleep()

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "lv_drivers/x11/x11.h"
#include "lv_drv_conf.h"
#include "lvgl/lvgl.h"

#define DISP_HOR_RES 320
#define DISP_VER_RES 240

static void hal_init(void);
static void hal_deinit(void);
static void *tick_thread(void *data);
static void lv_example_btn_1(void);

static pthread_t thr_tick;
static bool end_tick = false;

int main(void) {
  lv_init();

  hal_init();

  lv_example_btn_1();

  while (1) {
    lv_timer_handler();
    usleep(5 * 1000);
  }

  hal_deinit();
  return 0;
}

static void hal_init(void) {
  static lv_indev_drv_t indev_drv_1;
  lv_indev_drv_init(&indev_drv_1);
  indev_drv_1.type = LV_INDEV_TYPE_POINTER;

  static lv_indev_drv_t indev_drv_2;
  lv_indev_drv_init(&indev_drv_2);
  indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;

  static lv_indev_drv_t indev_drv_3;
  lv_indev_drv_init(&indev_drv_3);
  indev_drv_3.type = LV_INDEV_TYPE_ENCODER;

  lv_group_t *g = lv_group_create();
  lv_group_set_default(g);

  lv_disp_t *disp = NULL;

  lv_x11_init("OKDCC", DISP_HOR_RES, DISP_VER_RES);

  static lv_disp_draw_buf_t disp_buf1;
  static lv_color_t buf1_1[DISP_HOR_RES * 100];
  static lv_color_t buf1_2[DISP_HOR_RES * 100];
  lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, DISP_HOR_RES * 100);

  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.draw_buf = &disp_buf1;
  disp_drv.flush_cb = lv_x11_flush;
  disp_drv.hor_res = DISP_HOR_RES;
  disp_drv.ver_res = DISP_VER_RES;
  disp_drv.antialiasing = 1;

  disp = lv_disp_drv_register(&disp_drv);

  indev_drv_1.read_cb = lv_x11_get_pointer;
  indev_drv_2.read_cb = lv_x11_get_keyboard;
  indev_drv_3.read_cb = lv_x11_get_mousewheel;

  lv_theme_t *th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                         LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
  lv_disp_set_theme(disp, th);

  end_tick = false;
  pthread_create(&thr_tick, NULL, tick_thread, NULL);

  lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);
  lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
  lv_indev_t *enc_indev = lv_indev_drv_register(&indev_drv_3);
  lv_indev_set_group(kb_indev, g);
  lv_indev_set_group(enc_indev, g);

  LV_IMG_DECLARE(mouse_cursor_icon);
  lv_obj_t *cursor_obj = lv_img_create(lv_scr_act());
  lv_img_set_src(cursor_obj, &mouse_cursor_icon);
  lv_indev_set_cursor(mouse_indev, cursor_obj);
}

static void hal_deinit(void) {
  end_tick = true;
  pthread_join(thr_tick, NULL);

  lv_x11_deinit();
}

static void *tick_thread(void *data) {
  (void) data;

  while (!end_tick) {
    usleep(5000);
    lv_tick_inc(5);
  }

  return NULL;
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");
  } else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
  }
}

static void lv_example_btn_1(void) {
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
