#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

namespace buzzer
{
  void begin(uint8_t pin);
  void update();

  void beep(uint16_t onTime, uint16_t offTime, uint8_t count);
  void alarm(bool state);

  bool isActive();
}

#endif
