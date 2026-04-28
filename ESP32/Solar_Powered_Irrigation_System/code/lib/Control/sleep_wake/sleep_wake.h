#pragma once
#include <Arduino.h>

namespace sleep_wake
{
  void begin();
  void update();

  void requestSleep(bool state);
  bool wokeFromSleep();
}
