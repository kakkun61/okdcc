#define _DEFAULT_SOURCE  // necessary for usleep()

#include <lvgl.h>
#include <pthread.h>
#include <stdlib.h>
#include <ui.h>
#include <unistd.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

void readKeys(lv_indev_t *indev, lv_indev_data_t *data);

int main(void) {
  lv_init();

  lv_x11_window_create("OKDCC", SCREEN_WIDTH, SCREEN_HEIGHT);

  lv_indev_t *keysIndev = lv_indev_create();
  lv_indev_set_type(keysIndev, LV_INDEV_TYPE_KEYPAD);
  lv_indev_set_read_cb(keysIndev, readKeys);

  struct dcc_ui_Model_Command modelCommand = dcc_ui_init(keysIndev);
  dcc_ui_view(modelCommand.model);

  while (1) {
    lv_timer_handler();
    usleep(5 * 1000);
  }

  return 0;
}

void readKeys(lv_indev_t *indev, lv_indev_data_t *data) {
  // TODO
}
