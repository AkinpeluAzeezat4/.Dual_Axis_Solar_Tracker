#pragma once
#include <Arduino.h>

namespace button
{
  void begin();
  void update();

  bool wasPressed();
  bool wasLongPressed();
  bool isPressed();
}
