#include <Arduino.h>
#include "button.h"
#include "Pins.h"

namespace button
{
  static bool stableState = HIGH;
  static bool lastReading = HIGH;
  static bool pressEvent = false;
  static bool longPressEvent = false;
  static bool longPressDone = false;

  static unsigned long lastDebounce = 0;
  static unsigned long pressedAt = 0;

  static const unsigned long debounceDelay = 40;
  static const unsigned long longPressDelay = 2000;

  void begin()
  {
    pinMode(Pins::USER_BUTTON, INPUT_PULLUP);

    stableState = digitalRead(Pins::USER_BUTTON);
    lastReading = stableState;
  }

  void update()
  {
    bool reading = digitalRead(Pins::USER_BUTTON);
    unsigned long now = millis();

    if (reading != lastReading)
    {
      lastDebounce = now;
      lastReading = reading;
    }

    if (now - lastDebounce > debounceDelay)
    {
      if (reading != stableState)
      {
        stableState = reading;

        if (stableState == LOW)
        {
          pressedAt = now;
          longPressDone = false;
        }
        else
        {
          if (!longPressDone)
          {
            pressEvent = true;
          }
        }
      }
    }

    if (stableState == LOW && !longPressDone && now - pressedAt >= longPressDelay)
    {
      longPressDone = true;
      longPressEvent = true;
    }
  }

  bool wasPressed()
  {
    bool state = pressEvent;
    pressEvent = false;
    return state;
  }

  bool wasLongPressed()
  {
    bool state = longPressEvent;
    longPressEvent = false;
    return state;
  }

  bool isPressed()
  {
    return stableState == LOW;
  }
}
