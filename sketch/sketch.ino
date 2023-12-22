#include <Arduino.h>
#include <M5Stack.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "dcc.h"

volatile int eventTimesIndex = 0;
volatile unsigned long eventTimes[10] = { 0 };

void onClick(void) {
  if (eventTimesIndex >= 10) return;
  eventTimes[eventTimesIndex] = micros();
  eventTimesIndex++;
}

void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Lcd.fillScreen(WHITE);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("Start");

  Serial.begin(115200);

  attachInterrupt(39, onClick, CHANGE);
}

void loop() {
  M5.update();

  for (int i = 0; i < 10; i++) {
    if (eventTimes[i] == 0) break;
    Serial.printf("event time[%d]: %ld.%06ld\n", i, eventTimes[i] / 1000000, eventTimes[i] % 1000000);
  }
  for (int i = 0; i < 10; i++) {
    eventTimes[i] = 0;
  }
  eventTimesIndex = 0;
  delay(1000);
}
