#define _DEFAULT_SOURCE  // necessary for usleep()

#include <lvgl.h>
#include <okdcc/ui.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// void readKeys(lv_indev_t *indev, lv_indev_data_t *data);

int main(void) {
  lv_init();

  lv_display_t *display = lv_x11_window_create("OKDCC", SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_x11_inputs_create(display, NULL);

  static struct dcc_ui_Program program;
  dcc_ui_start(&program, NULL);

  // 実機の代わりに定期的に偽のパケットを流す
  unsigned long loopsCount = 0;
  while (1) {
    if (loopsCount % 100 == 0) {
      dcc_ui_dispatch(&program,
                      (struct dcc_ui_Message) { .tag = dcc_ui_PacketReceivedMessageTag,
                                                .packetText = "{\"tag\":\"dcc_IdlePacketForAllDecodersTag\"}" });
    }
    if (loopsCount % 700 == 0)
      dcc_ui_dispatch(&program, (struct dcc_ui_Message) { .tag = dcc_ui_DecodeFailedMessageTag, .packetText = NULL });
    loopsCount++;
    lv_timer_handler();
    usleep(5 * 1000);
  }

  return 0;
}
