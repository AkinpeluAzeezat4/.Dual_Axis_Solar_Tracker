#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <Arduino.h>

namespace keyboard_input
{
  void begin();
  void update();

  String getLastCommand();
}

#endif
