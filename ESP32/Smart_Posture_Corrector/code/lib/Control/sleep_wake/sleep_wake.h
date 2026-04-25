#pragma once
#include <Arduino.h>

namespace sleep_wake
{
  void begin();
  void update();

  void resetActivity();
  void sleepNow();

  bool isSleeping();
}
