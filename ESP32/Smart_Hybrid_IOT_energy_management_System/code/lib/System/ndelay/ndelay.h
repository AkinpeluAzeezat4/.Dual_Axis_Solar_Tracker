#pragma once

#include <Arduino.h>

namespace ndelay
{
  void begin();
  void update();

  bool millis_time(unsigned long timeFrame);
  bool millis_time(unsigned long &timer, unsigned long timeFrame);

  void millis_reset();
  void millis_reset(unsigned long &timer);
}
