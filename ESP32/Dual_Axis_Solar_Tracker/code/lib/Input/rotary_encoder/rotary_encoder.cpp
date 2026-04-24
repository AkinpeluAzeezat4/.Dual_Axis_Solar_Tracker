#include "rotary_encoder.h"
#include "pins.h"

namespace rotary_encoder
{
  int position = 0;
  int lastReported = 0;

  uint8_t lastEncoded = 0;

  bool pressed = false;
  bool lastButtonReading = HIGH;
  bool stableButtonState = HIGH;
  bool longPressDone = false;

  unsigned long lastDebounceTime = 0;
  unsigned long pressStartTime = 0;

  ButtonEvent eventState = NONE;

  const unsigned long debounceDelay = 40;
  const unsigned long longPressTime = 1200;

  void begin()
  {
    uint8_t a = Pins::readPin(Pins::ENC_CLK);
    uint8_t b = Pins::readPin(Pins::ENC_DT);

    lastEncoded = (a << 1) | b;
  }

  void update()
  {
    uint8_t a = Pins::readPin(Pins::ENC_CLK);
    uint8_t b = Pins::readPin(Pins::ENC_DT);

    uint8_t encoded = (a << 1) | b;
    uint8_t sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
      position++;

    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
      position--;

    lastEncoded = encoded;

    bool reading = Pins::readPin(Pins::ENC_SW);
    unsigned long now = millis();

    if (reading != lastButtonReading)
      lastDebounceTime = now;

    if ((now - lastDebounceTime) > debounceDelay)
    {
      if (reading != stableButtonState)
      {
        stableButtonState = reading;

        if (stableButtonState == LOW)
        {
          pressed = true;
          pressStartTime = now;
          longPressDone = false;
        }
        else
        {
          pressed = false;

          if (!longPressDone)
            eventState = SHORT_PRESS;
        }
      }
    }

    if (pressed && !longPressDone && (now - pressStartTime >= longPressTime))
    {
      longPressDone = true;
      eventState = LONG_PRESS;
    }

    lastButtonReading = reading;
  }

  int getPosition()
  {
    return position / 4;
  }

  int getDelta()
  {
    int current = getPosition();
    int delta = current - lastReported;
    lastReported = current;
    return delta;
  }

  bool isPressed()
  {
    return pressed;
  }

  ButtonEvent getButtonEvent()
  {
    return eventState;
  }

  void clearButtonEvent()
  {
    eventState = NONE;
  }
}
