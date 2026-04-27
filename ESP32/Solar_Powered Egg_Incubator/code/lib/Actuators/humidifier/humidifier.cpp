#include <Arduino.h>
#include "humidifier.h"

namespace humidifier
{
  static uint8_t relayPin = 255;
  static bool relayState = false;
  static bool relayActiveLow = true;

  static void apply()
  {
    if (relayPin == 255)
    {
      return;
    }

    digitalWrite(relayPin, relayActiveLow ? !relayState : relayState);
  }

  void begin(uint8_t pin, bool activeLow)
  {
    relayPin = pin;
    relayActiveLow = activeLow;
    relayState = false;

    pinMode(relayPin, OUTPUT);
    apply();
  }

  void update()
  {
  }

  void setState(bool state)
  {
    relayState = state;
    apply();
  }

  void toggle()
  {
    setState(!relayState);
  }

  bool isOn()
  {
    return relayState;
  }
}
