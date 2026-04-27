#ifndef HEATER_H
#define HEATER_H

#include <Arduino.h>

namespace heater
{
  void begin(uint8_t pin, bool activeLow = true);
  void update();

  void setState(bool state);
  void toggle();
  bool isOn();
}

#endif
