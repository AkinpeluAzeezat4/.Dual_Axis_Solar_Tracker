#include <Arduino.h>
#include "buzzer.h"
#include "Pins.h"

namespace buzzer
{
  static uint8_t BUZZER_PIN = Pins::BUZZER;
  static bool active = false;
  static bool beeping = false;

  static unsigned long beepStart = 0;
  static unsigned long beepDuration = 0;

  void begin()
  {
    pinMode(BUZZER_PIN, OUTPUT);
    Pins::writePin(BUZZER_PIN, LOW);
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
    Pins::writePin(BUZZER_PIN, HIGH);
  }

  void off()
  {
    active = false;
    Pins::writePin(BUZZER_PIN, LOW);
  }

  void beep(uint16_t duration_ms)
  {
    if (beeping)
      return;

    beepDuration = duration_ms;
    beepStart = millis();
    beeping = true;
    on();
  }

  bool isActive()
  {
    return active;
  }
}
