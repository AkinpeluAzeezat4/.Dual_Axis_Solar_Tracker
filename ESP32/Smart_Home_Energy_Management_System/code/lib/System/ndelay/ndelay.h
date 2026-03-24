#ifndef NDELAY_H
#define NDELAY_ENCODER_H

#include <Arduino.h>

namespace ndelay
{
  void begin();
  void update();
  void millis_reset();
  bool millis_time(unsigned long timeFrame);
}

#endif
