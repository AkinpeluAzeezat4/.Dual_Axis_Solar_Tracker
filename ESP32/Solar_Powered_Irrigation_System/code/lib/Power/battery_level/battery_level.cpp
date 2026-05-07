#include <Arduino.h>
#include "battery_level.h"
#include "Pins.h"
#include "settings_manager/settings_manager.h"

namespace battery_level
{
  static float voltage = 0.0f;
  static uint8_t percentage = 100;
  static bool ready = false;

  static unsigned long lastReadTime = 0;
  static const unsigned long readInterval = 1000;

  void begin()
  {
    pinMode(Pins::BATTERY_SENSE, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(Pins::BATTERY_SENSE, ADC_11db);
  }

  void update()
  {
    if (millis() - lastReadTime < readInterval)
    {
      return;
    }

    lastReadTime = millis();

    uint32_t sum = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
      sum += analogRead(Pins::BATTERY_SENSE);
    }

    float raw = sum / 16.0f;
    float adcVoltage = (raw / 4095.0f) * 3.3f;

    auto &s = settings_manager::get();
    voltage = adcVoltage * s.batteryScaleFactor;
    ready = true;

    if (s.batteryFullV <= s.batteryEmptyV)
    {
      percentage = 0;
      return;
    }

    float pct = ((voltage - s.batteryEmptyV) / (s.batteryFullV - s.batteryEmptyV)) * 100.0f;
    pct = constrain(pct, 0.0f, 100.0f);
    percentage = (uint8_t)(pct + 0.5f);
  }

  float getVoltage()
  {
    return voltage;
  }

  uint8_t getPercentage()
  {
    return percentage;
  }

  bool isReady()
  {
    return ready;
  }

  bool isLow()
  {
    auto &s = settings_manager::get();
    return ready && percentage <= s.batteryLowPercent;
  }
}
