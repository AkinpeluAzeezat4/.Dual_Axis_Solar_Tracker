#include <Arduino.h>
#include "solar_level.h"

namespace solar_level
{
  static uint8_t solarPin = 255;
  static float scaleFactor = 4.0f;
  static float voltage = 0.0f;
  static uint8_t percentage = 0;
  static unsigned long lastReadTime = 0;

  void begin(uint8_t pin, float scale)
  {
    solarPin = pin;
    scaleFactor = scale;

    analogReadResolution(12);
    analogSetPinAttenuation(solarPin, ADC_11db);
  }

  void update()
  {
    if (millis() - lastReadTime < 1000)
    {
      return;
    }

    lastReadTime = millis();

    uint32_t total = 0;

    for (uint8_t i = 0; i < 10; i++)
    {
      total += analogRead(solarPin);
    }

    float adcVoltage = ((total / 10.0f) / 4095.0f) * 3.3f;
    voltage = adcVoltage * scaleFactor;

    float pct = (voltage / 12.0f) * 100.0f;

    if (pct < 0)
    {
      pct = 0;
    }

    if (pct > 100)
    {
      pct = 100;
    }

    percentage = (uint8_t)pct;
  }

  float getVoltage()
  {
    return voltage;
  }

  uint8_t getPercentage()
  {
    return percentage;
  }
}
