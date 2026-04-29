#ifndef SLEEP_WAKE_H
#define SLEEP_WAKE_H

#include <Arduino.h>

namespace sleep_wake
{
  void begin(unsigned long timeoutMs = 30000);
  void update();

  void notifyActivity();
  void wake();

  bool isIdle();
  bool justEnteredIdle();
  bool justWoke();
  unsigned long getIdleTime();
}

#endif
