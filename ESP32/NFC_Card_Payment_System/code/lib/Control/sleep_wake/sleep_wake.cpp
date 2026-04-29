#include <Arduino.h>
#include "sleep_wake.h"

namespace
{
  unsigned long idleTimeout = 30000;
  unsigned long lastActivityTime = 0;

  bool idleState = false;
  bool enteredIdleFlag = false;
  bool wokeFlag = false;
}

namespace sleep_wake
{
  void begin(unsigned long timeoutMs)
  {
    idleTimeout = timeoutMs;
    lastActivityTime = millis();
    idleState = false;
    enteredIdleFlag = false;
    wokeFlag = false;
  }

  void update()
  {
    enteredIdleFlag = false;
    wokeFlag = false;

    unsigned long now = millis();

    if (!idleState && (now - lastActivityTime >= idleTimeout))
    {
      idleState = true;
      enteredIdleFlag = true;
    }
  }

  void notifyActivity()
  {
    bool wasIdle = idleState;

    lastActivityTime = millis();
    idleState = false;

    if (wasIdle)
    {
      wokeFlag = true;
    }
  }

  void wake()
  {
    notifyActivity();
  }

  bool isIdle()
  {
    return idleState;
  }

  bool justEnteredIdle()
  {
    return enteredIdleFlag;
  }

  bool justWoke()
  {
    return wokeFlag;
  }

  unsigned long getIdleTime()
  {
    return millis() - lastActivityTime;
  }
}
