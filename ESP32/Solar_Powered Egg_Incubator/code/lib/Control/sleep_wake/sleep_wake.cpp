#include <Arduino.h>
#include "sleep_wake.h"
#include "lcd_screen.h"

namespace sleep_wake
{
  static bool wakeFlag = false;

  void begin()
  {
    wakeFlag = false;
  }

  void update()
  {
  }

  void requestSleep(bool state)
  {
    if (state)
    {
      lcd_screen::sleep();
      wakeFlag = false;
    }
    else
    {
      lcd_screen::wake();
      wakeFlag = true;
    }
  }

  bool wokeFromSleep()
  {
    bool state = wakeFlag;
    wakeFlag = false;
    return state;
  }
}
