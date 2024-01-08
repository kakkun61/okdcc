#define _DEFAULT_SOURCE // necessary for usleep()

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include <lvgl/lvgl.h>
#include <ui.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

static void view(void);

int main(void) {
  lv_init();

  lv_x11_window_create("OKDCC", SCREEN_WIDTH, SCREEN_HEIGHT);

  view();

  while (1) {
    lv_timer_handler();
    usleep(5 * 1000);
  }

  return 0;
}

static void event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);

  if (code == LV_EVENT_CLICKED) {
    LV_LOG_USER("Clicked");
  } else if (code == LV_EVENT_VALUE_CHANGED) {
    LV_LOG_USER("Toggled");
  }
}

static void view(void) {
  lv_obj_t *buttonLabelsContainer = lv_obj_create(lv_scr_act());
  lv_obj_set_size(buttonLabelsContainer, SCREEN_WIDTH, LV_SIZE_CONTENT);
  lv_obj_align(buttonLabelsContainer, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_flex_flow(buttonLabelsContainer, LV_FLEX_FLOW_ROW);

  lv_obj_t *leftButtonLabel = lv_label_create(buttonLabelsContainer);
  lv_obj_set_flex_grow(leftButtonLabel, 1);
  lv_label_set_text(leftButtonLabel, "Down");
  lv_obj_set_style_text_align(leftButtonLabel, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t *centerButtonLabel = lv_label_create(buttonLabelsContainer);
  lv_obj_set_flex_grow(centerButtonLabel, 1);
  lv_label_set_text(centerButtonLabel, "Up");
  lv_obj_set_style_text_align(centerButtonLabel, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t *rightButtonLabel = lv_label_create(buttonLabelsContainer);
  lv_obj_set_flex_grow(rightButtonLabel, 1);
  lv_label_set_text(rightButtonLabel, "Enter");
  lv_obj_set_style_text_align(rightButtonLabel, LV_TEXT_ALIGN_CENTER, 0);
}
