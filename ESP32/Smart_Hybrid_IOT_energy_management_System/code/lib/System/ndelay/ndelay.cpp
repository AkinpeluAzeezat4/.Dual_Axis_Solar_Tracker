#include <Arduino.h>
#include "ndelay.h"

namespace ndelay
{
  unsigned long startTime = 0;

  void begin()
  {
    startTime = 0;
  }

  void update()
  {
  }

  bool millis_time(unsigned long timeFrame)
  {
    unsigned long now = millis();

    if (startTime == 0)
      startTime = now;

    if (now - startTime >= timeFrame)
    {
      startTime = now;
      return true;
    }

    return false;
  }

  bool millis_time(unsigned long &timer, unsigned long timeFrame)
  {
    unsigned long now = millis();

    if (timer == 0)
      timer = now;

    if (now - timer >= timeFrame)
    {
      timer = now;
      return true;
    }

    return false;
  }

  void millis_reset()
  {
    startTime = 0;
  }

  void millis_reset(unsigned long &timer)
  {
    timer = 0;
  }
}
