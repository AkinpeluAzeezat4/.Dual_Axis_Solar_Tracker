#include <Arduino.h>
#include "buzzer.h"
#include "Pins.h"

namespace buzzer
{
  static bool active = false;
  static bool beeping = false;
  static unsigned long beepStart = 0;
  static unsigned long beepDuration = 0;

  void begin()
  {
    pinMode(Pins::BUZZER, OUTPUT);
    Pins::writePin(Pins::BUZZER, LOW);
  }

  void update()
  {
    if (beeping && millis() - beepStart >= beepDuration)
    {
      off();
      beeping = false;
    }
  }

  void on()
  {
    active = true;
    Pins::writePin(Pins::BUZZER, HIGH);
  }

  void off()
  {
    active = false;
    Pins::writePin(Pins::BUZZER, LOW);
  }

  void beep(uint16_t durationMs)
  {
    if (beeping)
    {
      return;
    }

    beepDuration = durationMs;
    beepStart = millis();
    beeping = true;
    on();
  }

  bool isActive()
  {
    return active;
  }
}
