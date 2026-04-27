#ifndef SPINNER_H
#define SPINNER_H

#include <Arduino.h>

namespace spinner
{
  void begin(uint8_t pin, bool activeLow = true);
  void update();

  void setState(bool state);
  void toggle();
  bool isOn();
}

#endif
