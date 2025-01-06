#define _DEFAULT_SOURCE  // necessary for usleep()

#include <lvgl.h>
#include <pthread.h>
#include <stdlib.h>
#include <okdcc/ui.h>
#include <unistd.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// void readKeys(lv_indev_t *indev, lv_indev_data_t *data);

int main(void) {
  lv_init();

  lv_display_t const *display = lv_x11_window_create("OKDCC", SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_x11_inputs_create(display, NULL);

  struct dcc_ui_Model_Command modelCommand = dcc_ui_init(NULL);
  dcc_ui_view(modelCommand.model);

  while (1) {
    lv_timer_handler();
    usleep(5 * 1000);
  }

  return 0;
}
