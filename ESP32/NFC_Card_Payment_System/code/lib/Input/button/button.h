#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

namespace button
{
  enum ButtonId
  {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    OK,
    COUNT,
    NONE = 255
  };

  void begin(uint8_t upPin, uint8_t downPin, uint8_t leftPin, uint8_t rightPin, uint8_t okPin, bool activeLow = true);
  void update();

  bool isPressed(ButtonId id);
  bool wasPressed(ButtonId id);
  ButtonId getLastPressed();
}

#endif
