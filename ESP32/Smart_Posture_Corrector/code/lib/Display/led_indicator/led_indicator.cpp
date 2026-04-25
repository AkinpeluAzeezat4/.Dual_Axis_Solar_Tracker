#include <Arduino.h>
#include "led_indicator.h"
#include "Pins.h"

namespace led_indicator
{
  static uint8_t batteryPercent = 0;

  static bool faultState = false;
  static bool breathingState = false;
  static bool blinkState = false;

  static unsigned long lastBlink = 0;
  static unsigned long lastBreathUpdate = 0;
  static unsigned long pauseStart = 0;

  static int brightness = 0;
  static bool rising = true;
  static int pulseCount = 0;
  static bool inPause = false;

  static const int pwmFreq = 10000;
  static const int pwmResolution = 8;

  static const uint8_t ledChannel1 = 2;
  static const uint8_t ledChannel2 = 3;
  static const uint8_t ledChannel3 = 4;
  static const uint8_t ledChannel4 = 5;

  static const unsigned long stepTime = 1;
  static const unsigned long pauseDuration = 1000;

  static void writeLevels(uint8_t level)
  {
    ledcWrite(ledChannel1, level >= 1 ? 255 : 0);
    ledcWrite(ledChannel2, level >= 2 ? 255 : 0);
    ledcWrite(ledChannel3, level >= 3 ? 255 : 0);
    ledcWrite(ledChannel4, level >= 4 ? 255 : 0);
  }

  static void writeAll(uint8_t value)
  {
    ledcWrite(ledChannel1, value);
    ledcWrite(ledChannel2, value);
    ledcWrite(ledChannel3, value);
    ledcWrite(ledChannel4, value);
  }

  static void updateBatteryLevel()
  {
    if (batteryPercent >= 80)
      writeLevels(4);
    else if (batteryPercent >= 55)
      writeLevels(3);
    else if (batteryPercent >= 30)
      writeLevels(2);
    else if (batteryPercent >= 10)
      writeLevels(1);
    else
      writeLevels(0);
  }

  static void updateFaultBlink()
  {
    unsigned long now = millis();

    if (now - lastBlink >= 250)
    {
      lastBlink = now;
      blinkState = !blinkState;
      writeAll(blinkState ? 255 : 0);
    }
  }

  static void updateBreathing()
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

    if (now - lastBreathUpdate < stepTime)
    {
      return;
    }

    lastBreathUpdate = now;

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

    writeAll(brightness);
  }

  void begin()
  {
    ledcSetup(ledChannel1, pwmFreq, pwmResolution);
    ledcSetup(ledChannel2, pwmFreq, pwmResolution);
    ledcSetup(ledChannel3, pwmFreq, pwmResolution);
    ledcSetup(ledChannel4, pwmFreq, pwmResolution);

    ledcAttachPin(Pins::LED_1, ledChannel1);
    ledcAttachPin(Pins::LED_2, ledChannel2);
    ledcAttachPin(Pins::LED_3, ledChannel3);
    ledcAttachPin(Pins::LED_4, ledChannel4);

    writeAll(0);
  }

  void update()
  {
    if (faultState)
    {
      updateFaultBlink();
      return;
    }

    if (breathingState)
    {
      updateBreathing();
      return;
    }

    updateBatteryLevel();
  }

  void setBatteryLevel(uint8_t percent)
  {
    batteryPercent = percent > 100 ? 100 : percent;
  }

  void setFault(bool state)
  {
    faultState = state;

    if (!faultState)
    {
      blinkState = false;
      writeAll(0);
    }
  }

  void setBreathing(bool state)
  {
    breathingState = state;

    if (breathingState)
    {
      brightness = 0;
      rising = true;
      pulseCount = 0;
      inPause = false;
      lastBreathUpdate = millis();
      writeAll(0);
    }
    else
    {
      writeAll(0);
    }
  }

  Mode getMode()
  {
    if (faultState)
      return FAULT_BLINK;
    if (breathingState)
      return BREATHING;
    return BATTERY_LEVEL;
  }
}
