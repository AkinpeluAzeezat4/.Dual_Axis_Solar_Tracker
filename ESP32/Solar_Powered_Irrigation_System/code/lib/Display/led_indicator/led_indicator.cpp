#include <Arduino.h>
#include "led_indicator.h"
#include "Pins.h"

namespace led_indicator
{
  static const uint8_t ledPin = Pins::LED;
  static const int pwmChannel = 0;
  static const int pwmFreq = 10000;
  static const int pwmResolution = 8;

  static int brightness = 0;
  static bool rising = true;

  static unsigned long lastUpdate = 0;
  static const int stepTime = 1;

  static int pulseCount = 0;

  static bool inPause = false;
  static unsigned long pauseStart = 0;
  static const int pauseDuration = 1000;

  void begin()
  {
    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(ledPin, pwmChannel);
    ledcWrite(pwmChannel, 0);
  }

  void update()
  {
    unsigned long now = millis();

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
      brightness++;
      if (brightness >= 255)
      {
        brightness = 255;
        rising = false;
      }
    }
    else
    {
      brightness--;
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

  void on()
  {
    ledcWrite(pwmChannel, 255);
  }

  void off()
  {
    ledcWrite(pwmChannel, 0);
  }
}
