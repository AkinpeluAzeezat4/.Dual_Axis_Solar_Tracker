#pragma once
#include <Arduino.h>

namespace battery_level
{
  void begin();
  void update();

  float voltage();
  uint8_t percentage();

  bool isLow();
}
