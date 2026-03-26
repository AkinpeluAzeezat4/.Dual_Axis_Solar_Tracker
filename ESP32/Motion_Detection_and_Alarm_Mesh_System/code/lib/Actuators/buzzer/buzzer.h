#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "Pins.h"

namespace buzzer
{

  void begin(int pin);
  void update();

  void on();
  void off();
  void beep(uint16_t duration_ms);

}

#endif
