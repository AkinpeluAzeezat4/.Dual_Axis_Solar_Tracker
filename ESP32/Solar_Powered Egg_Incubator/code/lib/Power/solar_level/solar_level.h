#ifndef SOLAR_LEVEL_H
#define SOLAR_LEVEL_H

#include <Arduino.h>

namespace solar_level
{
  void begin(uint8_t pin, float scale);
  void update();

  float getVoltage();
  uint8_t getPercentage();
}

#endif
