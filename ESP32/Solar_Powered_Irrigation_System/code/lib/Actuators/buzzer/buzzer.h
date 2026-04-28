#pragma once
#include <Arduino.h>

namespace buzzer
{
  void begin();
  void update();
  void on();
  void off();
  void beep(uint16_t duration_ms);
  bool isActive();
}
