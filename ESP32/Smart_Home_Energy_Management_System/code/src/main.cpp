#include <Arduino.h>
// #include "Pins.h""

#include "led_indicator/led_indicator.h"

void setup()
{
  Serial.begin(115200);
  led_indicator::begin(2);
}

void loop()
{
  unsigned long now = millis();
  led_indicator::update();
}
