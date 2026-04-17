#include "buzzer/buzzer.h"
#include "Pins.h"
#include <Arduino.h>
#include "buzzer.h"

namespace buzzer
{
  static uint8_t BUZZER_PIN = 255;
  bool active = false;
  bool beeping = false;
  bool alarmRunning = false;

  unsigned long beepStart = 0;
  unsigned long beepDuration = 0;

  void begin(uint8_t pin)
  {
    BUZZER_PIN = pin;
    pinMode(BUZZER_PIN, OUTPUT);
    Pins::writePin(BUZZER_PIN, LOW);
  }

  void update()
  {
    if (beeping)
    {
      if (millis() - beepStart >= beepDuration)
      {
        off();
        beeping = false;
      }
    }
  }

  void on()
  {
    active = true;
    Pins::writePin(BUZZER_PIN, true);
  }

  void off()
  {
    active = false;
    Pins::writePin(BUZZER_PIN, false);
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
}
