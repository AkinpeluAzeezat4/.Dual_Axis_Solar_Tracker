#include <Arduino.h>
#include "vibration_motor.h"
#include "Pins.h"

namespace vibration_motor
{
  static const uint8_t channel = 0;

  static bool alertState = false;
  static bool outputState = false;
  static bool singlePulse = false;

  static unsigned long pulseStart = 0;
  static unsigned long lastToggle = 0;
  static uint16_t pulseDuration = 0;

  static void writeOutput(bool state)
  {
    outputState = state;
    ledcWrite(channel, state ? 220 : 0);
  }

  void begin()
  {
    ledcSetup(channel, 5000, 8);
    ledcAttachPin(Pins::VIBRATION_MOTOR, channel);
    writeOutput(false);
  }

  void update()
  {
    unsigned long now = millis();

    if (singlePulse)
    {
      if (now - pulseStart >= pulseDuration)
      {
        singlePulse = false;
        writeOutput(false);
      }
      return;
    }

    if (!alertState)
    {
      writeOutput(false);
      return;
    }

    if (now - lastToggle >= 500)
    {
      lastToggle = now;
      writeOutput(!outputState);
    }
    writeOutput(true);
  }

  void setAlert(bool state)
  {
    alertState = state;

    if (!alertState)
    {
      singlePulse = false;
      writeOutput(false);
    }
  }

  void pulse(uint16_t durationMs)
  {
    singlePulse = true;
    pulseStart = millis();
    pulseDuration = durationMs;
    writeOutput(true);
  }

  bool isActive()
  {
    return outputState;
  }
}
