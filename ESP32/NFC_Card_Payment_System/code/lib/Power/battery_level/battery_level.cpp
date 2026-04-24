#include <Arduino.h>
#include "battery_level.h"

namespace battery_level
{
  static uint8_t batteryPin = 255;
  static float voltage = 0.0f;
  static float scaleFactor = 1.61f;

  static unsigned long lastReadTime = 0;
  static const unsigned long readInterval = 500;

  void begin(uint8_t pin, float scale)
  {
    batteryPin = pin;
    scaleFactor = scale;

    analogReadResolution(12);
    analogSetPinAttenuation(batteryPin, ADC_11db);
  }

  void update()
  {
    unsigned long now = millis();

    if (now - lastReadTime >= readInterval)
    {
      lastReadTime = now;

      int raw = analogRead(batteryPin);
      float adcVoltage = (raw / 4095.0f) * 3.3f;
      voltage = adcVoltage * scaleFactor;
    }
  }

  float getVoltage()
  {
    return voltage;
  }

  uint8_t getPercentage()
  {
    float percent = ((voltage - 3.0f) / (4.2f - 3.0f)) * 100.0f;

    if (percent < 0.0f)
      percent = 0.0f;
    if (percent > 100.0f)
      percent = 100.0f;

    return (uint8_t)percent;
  }
}
