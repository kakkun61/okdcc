#include <Arduino.h>
#include <M5Unified.h>
#include <okdcc/electric.h>

void onButtonARising();
void onButtonBRising();
void onButtonCRising();

#define CHATTER_DELAY 500

struct buttonInterrupt {
  uint8_t pin;
  void (*callback)();
};

struct buttonInterrupt const buttonInterrupts[3] = {
  { 39, onButtonARising },
  { 38, onButtonBRising },
  { 37, onButtonCRising },
};

void setup() {
  M5.begin();
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    if (NOT_AN_INTERRUPT == digitalPinToInterrupt(buttonInterrupts[i].pin)) {
      Serial.print("GPIO ");
      Serial.print(buttonInterrupts[i].pin);
      Serial.println(" is not an interrupt pin");
    }
    attachInterrupt(buttonInterrupts[i].pin, buttonInterrupts[i].callback, FALLING);
  }
}

void loop() { M5.update(); }

void onButtonARising() {
  unsigned long static previousMillis = 0;
  unsigned long now = millis();
  if (now - previousMillis < CHATTER_DELAY) return;
  previousMillis = now;
  Serial.println("Button A pressed");
}

void onButtonBRising() {
  unsigned long static previousMillis = 0;
  unsigned long now = millis();
  if (now - previousMillis < CHATTER_DELAY) return;
  previousMillis = now;
  Serial.println("Button B pressed");
}

void onButtonCRising() {
  unsigned long static previousMillis = 0;
  unsigned long now = millis();
  if (now - previousMillis < CHATTER_DELAY) return;
  previousMillis = now;
  Serial.println("Button C pressed");
}
