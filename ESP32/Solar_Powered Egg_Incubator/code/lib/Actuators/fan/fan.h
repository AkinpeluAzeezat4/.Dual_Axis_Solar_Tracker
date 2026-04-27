#ifndef FAN_H
#define FAN_H

#include <Arduino.h>

namespace fan
{
  void begin(uint8_t pin, bool activeLow = true);
  void update();

  void on();
  void off();
  void set(bool state);
  bool isOn();
}

#endif
