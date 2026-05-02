#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

namespace buzzer
{
  void begin(uint8_t buzzerPin);
  void update();

  void on();
  void off();

  void beep(uint16_t durationMs = 100);
  void doubleBeep();
  void errorBeep();

  void startAlarm();
  void stopAlarm();

  bool isActive();
  bool isAlarmRunning();
}

#endif
