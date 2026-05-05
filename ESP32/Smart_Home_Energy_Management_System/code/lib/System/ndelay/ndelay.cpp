#include "ndelay.h"

namespace ndelay
{
  unsigned long startTime = 0;

  void begin()
  {
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

  void millis_reset()
  {
    startTime = 0;
  }
}
