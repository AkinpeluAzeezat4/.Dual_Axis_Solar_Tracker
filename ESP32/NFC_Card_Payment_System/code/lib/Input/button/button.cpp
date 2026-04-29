#include <Arduino.h>
#include "button.h"

namespace
{
  struct ButtonState
  {
    uint8_t pin = 255;
    bool pressed = false;
    bool edge = false;
    bool rawLast = false;
    unsigned long changedAt = 0;
  };

  ButtonState buttons[button::COUNT];
  bool useActiveLow = true;
  const unsigned long debounceMs = 35;
  button::ButtonId lastPressed = button::NONE;

  bool decode(uint8_t pin)
  {
    bool level = digitalRead(pin);
    return useActiveLow ? (!level) : level;
  }
}

namespace button
{
  void begin(uint8_t upPin, uint8_t downPin, uint8_t leftPin, uint8_t rightPin, uint8_t okPin, bool activeLow)
  {
    useActiveLow = activeLow;

    buttons[UP].pin = upPin;
    buttons[DOWN].pin = downPin;
    buttons[LEFT].pin = leftPin;
    buttons[RIGHT].pin = rightPin;
    buttons[OK].pin = okPin;

    for (uint8_t i = 0; i < COUNT; ++i)
    {
      pinMode(buttons[i].pin, INPUT);
      buttons[i].rawLast = decode(buttons[i].pin);
      buttons[i].pressed = buttons[i].rawLast;
      buttons[i].edge = false;
      buttons[i].changedAt = millis();
    }
  }

  void update()
  {
    lastPressed = NONE;

    for (uint8_t i = 0; i < COUNT; ++i)
    {
      bool raw = decode(buttons[i].pin);

      if (raw != buttons[i].rawLast)
      {
        buttons[i].rawLast = raw;
        buttons[i].changedAt = millis();
      }

      if ((millis() - buttons[i].changedAt) >= debounceMs)
      {
        if (buttons[i].pressed != raw)
        {
          buttons[i].pressed = raw;

          if (raw)
          {
            buttons[i].edge = true;
            lastPressed = static_cast<ButtonId>(i);
          }
        }
      }
    }
  }

  bool isPressed(ButtonId id)
  {
    if (id >= COUNT)
    {
      return false;
    }

    return buttons[id].pressed;
  }

  bool wasPressed(ButtonId id)
  {
    if (id >= COUNT)
    {
      return false;
    }

    bool hit = buttons[id].edge;
    buttons[id].edge = false;
    return hit;
  }

  ButtonId getLastPressed()
  {
    return lastPressed;
  }
}
