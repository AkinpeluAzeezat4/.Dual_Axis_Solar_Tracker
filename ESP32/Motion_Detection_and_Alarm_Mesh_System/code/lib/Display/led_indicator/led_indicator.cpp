#include <Arduino.h>
#include "Pins.h"
#include "led_indicator.h"

namespace led_indicator
{

  int ledPin;
  const int pwmChannel = 0;
  const int pwmFreq = 10000;
  const int pwmResolution = 8;

  int brightness = 0;
  bool rising = true;

  unsigned long lastUpdate = 0;
  const int stepTime = 1;

  int pulseCount = 0;

  bool inPause = false;
  unsigned long pauseStart = 0;
  const int pauseDuration = 1000;

  void begin(int pin)
  {
    ledPin = pin;

    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(ledPin, pwmChannel);
  }

  void update()
  {
    unsigned long now = millis();

    // handle pause state (non-blocking)
    if (inPause)
    {
      if (now - pauseStart >= pauseDuration)
      {
        inPause = false;
      }
      else
      {
        return;
      }
    }

    if (now - lastUpdate < stepTime)
      return;

    lastUpdate = now;

    if (rising)
    {
      brightness += 1;

      if (brightness >= 255)
      {
        brightness = 255;
        rising = false;
      }
    }
    else
    {
      brightness -= 1;

      if (brightness <= 0)
      {
        brightness = 0;
        rising = true;
        pulseCount++;

        if (pulseCount >= 2)
        {
          pulseCount = 0;
          inPause = true;
          pauseStart = now;
        }
      }
    }

    ledcWrite(pwmChannel, brightness);
  }

}
