#include <Arduino.h>
#include "battery_level.h"
#include "Pins.h"

namespace battery_level
{
  static float voltage = 0.0f;
  static uint8_t percentage = 0;
  static unsigned long lastUpdate = 0;

  static const unsigned long updateInterval = 500;

  static const float adcRef = 3.3f;
  static const float adcMax = 4095.0f;
  static const float dividerRatio = 2.0f;

  static const float batteryMin = 3.20f;
  static const float batteryMax = 4.20f;

  static uint8_t voltageToPercent(float v)
  {
    if (v <= batteryMin)
      return 0;
    if (v >= batteryMax)
      return 100;

    return (uint8_t)(((v - batteryMin) / (batteryMax - batteryMin)) * 100.0f);
  }

  static void readBattery()
  {
    uint32_t sum = 0;

    for (uint8_t i = 0; i < 20; i++)
    {
      sum += analogRead(Pins::BATTERY_ADC);
    }

    float raw = sum / 20.0f;
    voltage = (raw * adcRef / adcMax) * dividerRatio;
    Serial.println(voltage);
    percentage = voltageToPercent(voltage);
  }

  void begin()
  {
    analogReadResolution(12);
    analogSetPinAttenuation(Pins::BATTERY_ADC, ADC_11db);

    readBattery();
    lastUpdate = millis();
  }

  void update()
  {
    unsigned long now = millis();

    if (now - lastUpdate < updateInterval)
    {
      return;
    }

    lastUpdate = now;
    readBattery();
  }

  float getVoltage()
  {
    return voltage;
  }

  uint8_t getPercentage()
  {
    return percentage;
  }

  bool isLow()
  {
    return percentage <= 20;
  }
}
