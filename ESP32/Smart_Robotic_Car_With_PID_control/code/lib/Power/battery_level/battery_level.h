#ifndef BATTERY_LEVEL_H
#define BATTERY_LEVEL_H

#include <stdint.h>

namespace battery_level
{
  void begin();
  void update();

  float getVoltage();
  uint8_t getPercentage();
  bool isLow();
}

#endif
