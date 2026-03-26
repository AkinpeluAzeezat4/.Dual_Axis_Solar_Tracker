#include <Arduino.h>
#include "Pins.h"
#include "buzzer/buzzer.h"
#include "led_indicator/led_indicator.h"
void setup()
{
  Serial.begin(115200);
  led_indicator::begin(Pins::GPIO2);
  // pinMode(23, OUTPUT);
  buzzer::begin(Pins::GPIO23);
  buzzer::beep(2000);
}

void loop()
{
  led_indicator::update();
  // digitalWrite(23,HIGH);
  buzzer::update();
}
