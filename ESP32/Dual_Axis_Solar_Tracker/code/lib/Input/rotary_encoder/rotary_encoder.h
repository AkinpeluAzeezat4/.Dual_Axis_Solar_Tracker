#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>

namespace rotary_encoder
{
  enum ButtonEvent
  {
    NONE,
    SHORT_PRESS,
    LONG_PRESS
  };

  void begin();
  void update();

  int getPosition();
  int getDelta();

  bool isPressed();

  ButtonEvent getButtonEvent();
  void clearButtonEvent();
}

#endif
