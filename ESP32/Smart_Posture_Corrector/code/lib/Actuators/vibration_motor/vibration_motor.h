#pragma once
#include <Arduino.h>

namespace vibration_motor
{
  void begin();
  void update();

  void setAlert(bool state);
  void pulse(uint16_t durationMs);

  bool isActive();
}
