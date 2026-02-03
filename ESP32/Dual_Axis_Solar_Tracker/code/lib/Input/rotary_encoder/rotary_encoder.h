#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <Arduino.h>
#include "Pins.h"

namespace rotary_encoder
{

  void begin();
  void update();
  int getPosition();
  bool isPressed();

}

#endif
