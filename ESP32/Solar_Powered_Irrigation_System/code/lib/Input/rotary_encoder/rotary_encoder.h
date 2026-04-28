#pragma once
#include <Arduino.h>

namespace rotary_encoder
{
  void begin();
  void update();

  int getPosition();
  int getDelta();

  bool isPressed();
  bool wasClicked();
  bool wasLongPressed();
}
