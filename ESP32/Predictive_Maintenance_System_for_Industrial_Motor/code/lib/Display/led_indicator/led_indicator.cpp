#include <Arduino.h>
#include "Pins.h"
#include "led_indicator.h"
#include "load_relay/load_relay.h"

namespace led_indicator
{
  static const uint8_t ledPin = Pins::STATUS_LED;

  static const uint8_t pwmChannel = 0;
  static const uint16_t pwmFreq = 10000;
  static const uint8_t pwmResolution = 8;

  static uint8_t brightness = 0;
  static bool rising = true;
  static bool state = false;

  static unsigned long lastUpdate = 0;
  static const uint16_t stepTime = 4;

  static uint8_t pulseCount = 0;
  static bool inPause = false;
  static unsigned long pauseStart = 0;
  static const uint16_t pauseDuration = 1000;

  static bool forcedMode = false;

  static void writeBrightness(uint8_t value)
  {
    brightness = value;
    state = brightness > 0;
    ledcWrite(pwmChannel, brightness);
  }

  void begin()
  {
    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(ledPin, pwmChannel);
    writeBrightness(0);
  }

  void update()
  {
    if (forcedMode)
    {
      return;
    }

    unsigned long now = millis();

    if (load_relay::isFault())
    {
      static unsigned long lastFaultBlink = 0;
      static bool faultState = false;

      if (now - lastFaultBlink >= 150)
      {
        lastFaultBlink = now;
        faultState = !faultState;
        writeBrightness(faultState ? 255 : 0);
      }

      return;
    }

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
    {
      return;
    }

    lastUpdate = now;

    if (rising)
    {
      if (brightness < 255)
      {
        brightness++;
      }

      if (brightness >= 255)
      {
        brightness = 255;
        rising = false;
      }
    }
    else
    {
      if (brightness > 0)
      {
        brightness--;
      }

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

    writeBrightness(brightness);
  }

  void on()
  {
    forcedMode = true;
    writeBrightness(255);
  }

  void off()
  {
    forcedMode = true;
    writeBrightness(0);
  }

  bool isOn()
  {
    return state;
  }
}
