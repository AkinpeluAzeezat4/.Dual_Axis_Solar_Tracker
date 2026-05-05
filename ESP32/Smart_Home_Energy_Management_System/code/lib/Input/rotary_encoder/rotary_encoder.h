#pragma once

#include <Arduino.h>

namespace rotary_encoder
{
  void begin();
  void update();

  int getDirection();

  bool wasPressed();
  bool isPressed();
  bool isHeld(unsigned long holdTime);

  void lockUntilRelease();
}
