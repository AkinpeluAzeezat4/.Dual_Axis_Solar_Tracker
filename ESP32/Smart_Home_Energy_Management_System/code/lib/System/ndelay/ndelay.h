#pragma once
#include <Arduino.h>

namespace ndelay
{
  void begin();
  void update();
  bool millis_time(unsigned long timeFrame);
  void millis_reset();
}
