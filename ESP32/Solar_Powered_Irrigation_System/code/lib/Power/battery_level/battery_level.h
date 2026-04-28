#pragma once
#include <Arduino.h>

namespace battery_level
{
  void begin(uint8_t pin, float scale);
  void update();

  float getVoltage();
}
