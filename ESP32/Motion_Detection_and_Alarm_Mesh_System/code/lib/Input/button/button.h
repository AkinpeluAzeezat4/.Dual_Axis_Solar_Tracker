#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

namespace button
{
  void begin(uint8_t pin);
  void update();

  bool isPressed(); // raw state (held down)
  bool isTrigger(); // one-shot trigger (like PIR event)
}

#endif
