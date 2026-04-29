#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

namespace buzzer
{
  void begin(uint8_t pin);
  void update();

  void on();
  void off();
  void beep(uint16_t durationMs);

  void playSuccess();
  void playFailure();
  void playRegister();

  bool isActive();
}

#endif
