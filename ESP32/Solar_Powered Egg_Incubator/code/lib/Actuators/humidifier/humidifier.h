#ifndef HUMIDIFIER_H
#define HUMIDIFIER_H

#include <Arduino.h>

namespace humidifier
{
  void begin(uint8_t pin, bool activeLow = true);
  void update();

  void setState(bool state);
  void toggle();
  bool isOn();
}

#endif
