#include <Arduino.h>
#include "buzzer.h"
#include "Pins/Pins.h"

namespace buzzer
{
  static uint8_t pin = Pins::UNUSED_PIN;
  static bool active = false;
  static bool beeping = false;
  static bool alarmRunning = false;

  static unsigned long beepStart = 0;
  static unsigned long beepDuration = 0;

  static uint8_t pulseTarget = 0;
  static uint8_t pulseCount = 0;

  static bool pulseGap = false;
  static unsigned long gapStart = 0;

  static void resetPattern()
  {
    beeping = false;
    pulseGap = false;
    pulseTarget = 0;
    pulseCount = 0;
    beepDuration = 0;
    beepStart = 0;
    gapStart = 0;
  }

  void begin(uint8_t buzzerPin)
  {
    pin = buzzerPin;

    if (!Pins::valid(pin))
      return;

    pinMode(pin, OUTPUT);
    Pins::writePin(pin, LOW);
    active = false;
    resetPattern();
    alarmRunning = false;
  }

  void update()
  {
    if (!Pins::valid(pin))
      return;

    unsigned long now = millis();

    if (alarmRunning)
    {
      if ((now / 250) % 2 == 0)
        on();
      else
        off();

      return;
    }

    if (beeping && now - beepStart >= beepDuration)
    {
      off();
      beeping = false;
      pulseCount++;

      if (pulseCount < pulseTarget)
      {
        pulseGap = true;
        gapStart = now;
      }
      else
      {
        resetPattern();
      }
    }

    if (pulseGap && now - gapStart >= 120)
    {
      pulseGap = false;
      beeping = true;
      beepStart = now;
      on();
    }
  }

  void on()
  {
    if (!Pins::valid(pin))
      return;

    active = true;
    Pins::writePin(pin, HIGH);
  }

  void off()
  {
    if (!Pins::valid(pin))
      return;

    active = false;
    Pins::writePin(pin, LOW);
  }

  void beep(uint16_t durationMs)
  {
    if (!Pins::valid(pin) || alarmRunning)
      return;

    resetPattern();

    pulseTarget = 1;
    beepDuration = durationMs;
    beepStart = millis();
    beeping = true;

    on();
  }

  void doubleBeep()
  {
    if (!Pins::valid(pin) || alarmRunning)
      return;

    resetPattern();

    pulseTarget = 2;
    beepDuration = 100;
    beepStart = millis();
    beeping = true;

    on();
  }

  void errorBeep()
  {
    if (!Pins::valid(pin) || alarmRunning)
      return;

    resetPattern();

    pulseTarget = 3;
    beepDuration = 90;
    beepStart = millis();
    beeping = true;

    on();
  }

  void startAlarm()
  {
    if (!Pins::valid(pin))
      return;

    resetPattern();
    alarmRunning = true;
  }

  void stopAlarm()
  {
    alarmRunning = false;
    resetPattern();
    off();
  }

  bool isActive()
  {
    return active;
  }

  bool isAlarmRunning()
  {
    return alarmRunning;
  }
}
