#ifndef BATTERY_LEVEL_H
#define BATTERY_LEVEL_H

#include <Arduino.h>

namespace battery_level
{
  void begin(uint8_t pin, float scale = 4.615f);
  void update();

  float getVoltage();
  uint8_t getPercentage();
}

#endif
