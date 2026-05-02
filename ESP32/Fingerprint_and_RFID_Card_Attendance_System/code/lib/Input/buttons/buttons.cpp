#include <Arduino.h>
#include "buttons.h"
#include "Pins.h"

namespace buttons
{
  static const uint8_t pins[BUTTON_COUNT] = {
      Pins::BTN1,
      Pins::BTN2,
      Pins::BTN3,
      Pins::BTN4,
      Pins::BTN5};

  static bool stableState[BUTTON_COUNT] = {false, false, false, false, false};
  static bool lastRawState[BUTTON_COUNT] = {false, false, false, false, false};
  static bool pressEvent[BUTTON_COUNT] = {false, false, false, false, false};

  static unsigned long lastChange[BUTTON_COUNT] = {0, 0, 0, 0, 0};

  static const unsigned long debounceMs = 35;

  static bool readButton(uint8_t index)
  {
    if (index >= BUTTON_COUNT)
      return false;

    if (!Pins::valid(pins[index]))
      return false;

    return digitalRead(pins[index]) == LOW;
  }

  void begin()
  {
    for (uint8_t i = 0; i < BUTTON_COUNT; i++)
    {
      if (Pins::valid(pins[i]))
        pinMode(pins[i], INPUT_PULLUP);

      bool raw = readButton(i);

      stableState[i] = raw;
      lastRawState[i] = raw;
      pressEvent[i] = false;
      lastChange[i] = millis();
    }
  }

  void update()
  {
    unsigned long now = millis();

    for (uint8_t i = 0; i < BUTTON_COUNT; i++)
    {
      bool raw = readButton(i);

      if (raw != lastRawState[i])
      {
        lastRawState[i] = raw;
        lastChange[i] = now;
      }

      if ((now - lastChange[i]) >= debounceMs && raw != stableState[i])
      {
        stableState[i] = raw;

        if (stableState[i])
          pressEvent[i] = true;
      }
    }
  }

  bool isPressed(ButtonId button)
  {
    if (button >= BUTTON_COUNT)
      return false;

    return stableState[button];
  }

  bool wasPressed(ButtonId button)
  {
    if (button >= BUTTON_COUNT)
      return false;

    bool event = pressEvent[button];
    pressEvent[button] = false;

    return event;
  }
}
