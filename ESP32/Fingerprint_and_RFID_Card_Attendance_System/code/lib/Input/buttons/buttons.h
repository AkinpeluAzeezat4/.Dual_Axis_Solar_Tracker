#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

namespace buttons
{
  enum ButtonId
  {
    BUTTON_1 = 0,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_COUNT
  };

  void begin();
  void update();

  bool isPressed(ButtonId button);
  bool wasPressed(ButtonId button);
}

#endif
