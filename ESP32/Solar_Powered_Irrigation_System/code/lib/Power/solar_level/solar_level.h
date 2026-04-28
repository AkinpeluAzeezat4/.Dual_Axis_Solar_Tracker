#pragma once
#include <Arduino.h>

namespace solar_level
{
  void begin(uint8_t pin, float scale);
  void update();

  float getVoltage();
  bool isPresent();
}
