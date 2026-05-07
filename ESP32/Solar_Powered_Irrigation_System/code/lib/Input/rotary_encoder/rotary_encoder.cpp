#include <Arduino.h>
#include "rotary_encoder.h"
#include "Pins.h"

namespace rotary_encoder
{
  static int position = 0;
  static int lastPosition = 0;

  static bool lastA = HIGH;
  static bool stableButtonState = false;
  static bool lastReading = false;
  static bool clicked = false;
  static bool longPressed = false;
  static bool longPressLatched = false;

  static unsigned long pressStart = 0;
  static unsigned long lastButtonChange = 0;
  static const unsigned long debounceTime = 40;
  static const unsigned long longPressTime = 5000;

  void begin()
  {
    pinMode(Pins::ENCODER_CLK, INPUT_PULLUP);
    pinMode(Pins::ENCODER_DT, INPUT_PULLUP);
    pinMode(Pins::ENCODER_SW, INPUT_PULLUP);

    lastA = digitalRead(Pins::ENCODER_CLK);
    lastReading = digitalRead(Pins::ENCODER_SW) == LOW;
    stableButtonState = lastReading;
  }

  void update()
  {
    clicked = false;
    longPressed = false;

    bool A = digitalRead(Pins::ENCODER_CLK);
    bool B = digitalRead(Pins::ENCODER_DT);

    if (A != lastA)
    {
      if (A == B)
      {
        position++;
      }
      else
      {
        position--;
      }

      lastA = A;
    }

    bool reading = digitalRead(Pins::ENCODER_SW) == LOW;

    if (reading != lastReading)
    {
      lastReading = reading;
      lastButtonChange = millis();
    }

    if ((millis() - lastButtonChange) > debounceTime)
    {
      if (reading != stableButtonState)
      {
        stableButtonState = reading;

        if (stableButtonState)
        {
          pressStart = millis();
          longPressLatched = false;
        }
        else
        {
          unsigned long pressDuration = millis() - pressStart;

          if (pressDuration < longPressTime && !longPressLatched)
          {
            clicked = true;
          }
        }
      }
    }

    if (stableButtonState && !longPressLatched && millis() - pressStart >= longPressTime)
    {
      longPressLatched = true;
      longPressed = true;
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
    return stableButtonState;
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
