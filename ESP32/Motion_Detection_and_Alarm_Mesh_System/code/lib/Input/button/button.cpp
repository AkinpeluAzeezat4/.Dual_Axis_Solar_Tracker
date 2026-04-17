#include <Arduino.h>
#include "button.h"

namespace button
{
  static uint8_t buttonPin = 255;

  static bool currentState = HIGH;
  static bool lastStableState = HIGH;

  static unsigned long lastDebounceTime = 0;
  static const unsigned long debounceDelay = 50;

  static bool triggerEvent = false;

  void begin(uint8_t pin)
  {
    buttonPin = pin;
    pinMode(buttonPin, INPUT);

    currentState = digitalRead(buttonPin);
    lastStableState = currentState;
  }

  void update()
  {
    bool reading = digitalRead(buttonPin);

    if (reading != lastStableState)
    {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay)
    {
      if (reading != currentState)
      {
        currentState = reading;

        // Button pressed (LOW because of INPUT_PULLUP)
        if (currentState == LOW)
        {
          triggerEvent = true; // simulate motion trigger
        }
      }
    }

    lastStableState = reading;
  }

  bool isPressed()
  {
    return currentState == LOW;
  }

  bool isTrigger()
  {
    if (triggerEvent)
    {
      triggerEvent = false;
      return true;
    }
    return false;
  }
}
