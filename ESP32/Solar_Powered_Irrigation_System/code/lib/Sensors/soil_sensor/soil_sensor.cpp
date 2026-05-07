#include <Arduino.h>
#include "soil_sensor.h"
#include "Pins.h"
#include "settings_manager/settings_manager.h"

namespace soil_sensor
{
  static int rawValue = 0;
  static uint8_t moisturePercent = 0;
  static bool ready = false;

  static unsigned long lastRead = 0;
  static const unsigned long readInterval = 1000;

  void begin()
  {
    pinMode(Pins::SOIL_SENSOR, INPUT);
    analogSetPinAttenuation(Pins::SOIL_SENSOR, ADC_11db);
  }

  void update()
  {
    if (millis() - lastRead < readInterval)
    {
      return;
    }

    lastRead = millis();

    uint32_t sum = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
      sum += analogRead(Pins::SOIL_SENSOR);
    }

    rawValue = sum / 16;
    ready = true;

    auto &s = settings_manager::get();

    if (s.soilDryRaw == s.soilWetRaw)
    {
      moisturePercent = 0;
      return;
    }

    int percent = map(rawValue, s.soilDryRaw, s.soilWetRaw, 0, 100);
    percent = constrain(percent, 0, 100);
    moisturePercent = (uint8_t)percent;
  }

  int getRaw()
  {
    return rawValue;
  }

  uint8_t getMoisturePercent()
  {
    return moisturePercent;
  }

  bool isReady()
  {
    return ready;
  }

  bool isDry()
  {
    auto &s = settings_manager::get();
    return ready && moisturePercent <= s.soilDryPercent;
  }
}
