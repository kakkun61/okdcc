#include <M5Unified.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <okdcc/electric.h>

#define CHATTER_DELAY_MICRO_SEC (500 * 1000)
#define BUTTON_COUNT 3

extern "C" {
void app_main(void);
}

static void onButtonFalling(void *);

struct Button {
  char const *const name;
  gpio_num_t const pin;
  int64_t previousMicroSec;
};

static Button buttons[BUTTON_COUNT] = {
  { "A", GPIO_NUM_39, 0 },
  { "B", GPIO_NUM_38, 0 },
  { "C", GPIO_NUM_37, 0 },
};

static QueueHandle_t pressedButtons = NULL;

void app_main(void) {
  M5.begin();
  pressedButtons = xQueueCreate(8, sizeof(int));
  {
    esp_err_t const result = gpio_install_isr_service(0);
    if (ESP_OK != result) printf("Failed to install ISR service: %s\n", esp_err_to_name(result));
  }
  for (int i = 0; i < BUTTON_COUNT; i++) {
    {
      esp_err_t const result = gpio_set_intr_type(buttons[i].pin, GPIO_INTR_NEGEDGE);
      if (ESP_OK != result) {
        printf("Failed to set interrupt type on GPIO %d: %s\n", buttons[i].pin, esp_err_to_name(result));
      }
    }
    {
      esp_err_t const result = gpio_isr_handler_add(buttons[i].pin, onButtonFalling, (void *) (intptr_t) i);
      if (ESP_OK != result) {
        printf("Failed to add ISR handler on GPIO %d: %s\n", buttons[i].pin, esp_err_to_name(result));
      }
    }
  }
  for (;;) {
    M5.update();
    int i;
    while (pdTRUE == xQueueReceive(pressedButtons, &i, 0)) printf("Button %s pressed\n", buttons[i].name);
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void onButtonFalling(void *arg) {
  int const i = (int) (intptr_t) arg;
  int64_t const now = esp_timer_get_time();
  if (now - buttons[i].previousMicroSec < CHATTER_DELAY_MICRO_SEC) return;
  buttons[i].previousMicroSec = now;
  BaseType_t woken = pdFALSE;
  xQueueSendFromISR(pressedButtons, &i, &woken);
  if (woken) portYIELD_FROM_ISR();
}
