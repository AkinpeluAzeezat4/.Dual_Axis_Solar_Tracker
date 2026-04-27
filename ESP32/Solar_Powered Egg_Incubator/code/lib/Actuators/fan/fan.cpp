#include <Arduino.h>
#include "fan.h"

namespace fan
{
  static uint8_t fanPin = 255;
  static bool fanState = false;
  static bool relayActiveLow = true;

  static void writeOutput()
  {
    if (fanPin == 255)
    {
      return;
    }

    if (relayActiveLow)
    {
      digitalWrite(fanPin, fanState ? LOW : HIGH);
    }
    else
    {
      digitalWrite(fanPin, fanState ? HIGH : LOW);
    }
  }

  void begin(uint8_t pin, bool activeLow)
  {
    fanPin = pin;
    relayActiveLow = activeLow;
    fanState = false;

    pinMode(fanPin, OUTPUT);
    writeOutput();
  }

  void update()
  {
  }

  void on()
  {
    fanState = true;
    writeOutput();
  }

  void off()
  {
    fanState = false;
    writeOutput();
  }

  void set(bool state)
  {
    fanState = state;
    writeOutput();
  }

  bool isOn()
  {
    return fanState;
  }
}
