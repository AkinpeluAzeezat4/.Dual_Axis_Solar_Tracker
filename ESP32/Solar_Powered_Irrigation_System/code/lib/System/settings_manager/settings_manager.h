#pragma once
#include <Arduino.h>

namespace settings_manager
{
  struct Settings
  {
    float tankEmptyCm;
    float tankFullCm;

    uint16_t soilDryRaw;
    uint16_t soilWetRaw;

    float batteryEmptyV;
    float batteryFullV;

    uint16_t tankLowPercent;
    uint16_t tankHighPercent;
    uint16_t soilDryPercent;

    uint16_t irrigationRunSeconds;
    uint16_t irrigationPauseSeconds;
    uint16_t tankPumpMaxMinutes;

    bool valid;
  };

  void begin();
  void save();
  void resetDefaults();

  Settings &get();
}
