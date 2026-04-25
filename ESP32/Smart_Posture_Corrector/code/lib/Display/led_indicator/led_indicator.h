#pragma once
#include <Arduino.h>

namespace led_indicator
{
  enum Mode
  {
    BATTERY_LEVEL,
    BREATHING,
    FAULT_BLINK
  };

  void begin();
  void update();

  void setBatteryLevel(uint8_t percent);
  void setFault(bool state);
  void setBreathing(bool state);

  Mode getMode();
}
