#include <Arduino.h>
#include "solar_level.h"

namespace solar_level
{
  static uint8_t solarPin = 255;
  static float voltage = 0.0f;
  static float scaleFactor = 2.0f;

  static unsigned long lastReadTime = 0;
  static const unsigned long readInterval = 500;

  void begin(uint8_t pin, float scale)
  {
    solarPin = pin;
    scaleFactor = scale;

    pinMode(solarPin, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(solarPin, ADC_11db);
  }

  void update()
  {
    if (solarPin == 255)
      return;

    unsigned long now = millis();

    if (now - lastReadTime >= readInterval)
    {
      lastReadTime = now;

      int raw = analogRead(solarPin);
      float adcVoltage = (raw / 4095.0f) * 3.3f;
      voltage = adcVoltage * scaleFactor;
    }
  }

  float getVoltage()
  {
    return voltage;
  }

  bool isPresent()
  {
    return voltage > 1.0f;
  }
}
