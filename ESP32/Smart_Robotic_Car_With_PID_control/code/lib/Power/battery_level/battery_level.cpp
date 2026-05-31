#include <Arduino.h>
#include "battery_level.h"
#include "Pins.h"

namespace battery_level
{
  static float voltage = 0.0f;
  static uint8_t percentage = 0;
  static unsigned long lastReadTime = 0;
  static const unsigned long readInterval = 500;
  static const float adcRef = 3.3f;
  static const float dividerRatio = 3.8f;
  static const float minVoltage = 10.0f;
  static const float maxVoltage = 12.6f;
  static uint8_t voltageToPercent(float v)
  {
    if (v <= minVoltage)
      return 0;
    if (v >= maxVoltage)
      return 100;
    return (uint8_t)(((v - minVoltage) * 100.0f) / (maxVoltage - minVoltage));
  }

  void begin()
  {
    analogReadResolution(12);
    analogSetPinAttenuation(Pins::BATTERY_ADC, ADC_11db);
  }

  void update()
  {
    if (millis() - lastReadTime < readInterval)
      return;
    lastReadTime = millis();

    uint32_t sum = 0;
    for (uint8_t i = 0; i < 20; i++)
    {
      sum += analogRead(Pins::BATTERY_ADC);
    }

    float raw = sum / 20.0f;
    float adcVoltage = (raw / 4095.0f) * adcRef;

    voltage = adcVoltage * dividerRatio;
    percentage = voltageToPercent(voltage);
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
