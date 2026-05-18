#include <Arduino.h>
#include "led_indicator.h"

namespace led_indicator
{
  int ledPin = 2;

  const int pwmChannel = 7;
  const int pwmFreq = 5000;
  const int pwmResolution = 8;

  int brightness = 0;
  bool rising = true;

  unsigned long lastUpdate = 0;
  const unsigned long stepTime = 4;

  int pulseCount = 0;

  bool inPause = false;
  unsigned long pauseStart = 0;
  const unsigned long pauseDuration = 700;

  bool enabled = true;

  void begin(int pin)
  {
    ledPin = pin;

    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(ledPin, pwmChannel);
    ledcWrite(pwmChannel, 0);
  }

  void update()
  {
    if (!enabled)
    {
      ledcWrite(pwmChannel, 0);
      return;
    }

    unsigned long now = millis();

    if (inPause)
    {
      if (now - pauseStart < pauseDuration)
        return;

      inPause = false;
    }

    if (now - lastUpdate < stepTime)
      return;

    lastUpdate = now;

    if (rising)
    {
      brightness += 8;

      if (brightness >= 255)
      {
        brightness = 255;
        rising = false;
      }
    }
    else
    {
      brightness -= 10;

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

  void setEnabled(bool state)
  {
    enabled = state;

    if (!enabled)
    {
      brightness = 0;
      rising = true;
      pulseCount = 0;
      inPause = false;
      ledcWrite(pwmChannel, 0);
    }
  }

  bool isEnabled()
  {
    return enabled;
  }
}
