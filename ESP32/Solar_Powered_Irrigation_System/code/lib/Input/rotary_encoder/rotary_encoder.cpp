#include <Arduino.h>
#include "rotary_encoder.h"
#include "Pins.h"

namespace rotary_encoder
{
  static int position = 0;
  static int lastPosition = 0;

  static bool lastA = HIGH;
  static bool buttonState = false;
  static bool lastButtonState = false;
  static bool clicked = false;
  static bool longPressed = false;

  static unsigned long pressStart = 0;
  static const unsigned long debounceTime = 40;
  static const unsigned long longPressTime = 5000;
  static unsigned long lastButtonChange = 0;

  void begin()
  {
    pinMode(Pins::ENC_CLK, INPUT_PULLUP);
    pinMode(Pins::ENC_DT, INPUT_PULLUP);
    pinMode(Pins::ENC_SW, INPUT_PULLUP);

    lastA = digitalRead(Pins::ENC_CLK);
  }

  void update()
  {
    clicked = false;
    longPressed = false;

    bool A = digitalRead(Pins::ENC_CLK);
    bool B = digitalRead(Pins::ENC_DT);

    if (A != lastA)
    {
      if (A == B)
        position++;
      else
        position--;

      lastA = A;
    }

    bool reading = digitalRead(Pins::ENC_SW) == LOW;

    if (reading != lastButtonState)
    {
      lastButtonChange = millis();
      lastButtonState = reading;
    }

    if ((millis() - lastButtonChange) > debounceTime)
    {
      if (reading != buttonState)
      {
        buttonState = reading;

        if (buttonState)
        {
          pressStart = millis();
        }
        else
        {
          unsigned long pressDuration = millis() - pressStart;

          if (pressDuration >= longPressTime)
            longPressed = true;
          else
            clicked = true;
        }
      }
    }
  }

  int getPosition()
  {
    return position;
  }

  int getDelta()
  {
    int delta = position - lastPosition;
    lastPosition = position;
    return delta;
  }

  bool isPressed()
  {
    return buttonState;
  }

  bool wasClicked()
  {
    bool state = clicked;
    clicked = false;
    return state;
  }

  bool wasLongPressed()
  {
    bool state = longPressed;
    longPressed = false;
    return state;
  }
}
