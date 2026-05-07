#pragma once
#include <Arduino.h>

namespace system_logic
{
  enum RunState
  {
    STARTING,
    NORMAL_IDLE,
    FILLING_TANK,
    TANK_LOW_LOCKOUT,
    IRRIGATING,
    IRRIGATION_PAUSE,
    SENSOR_FAULT,
    TANK_PUMP_TIMEOUT,
    BATTERY_LOW
  };

  void begin();
  void update();

  RunState getState();
  const char *getStatusText();
  bool isTankFillActive();
  bool isIrrigationActive();
  bool hasFault();
}
