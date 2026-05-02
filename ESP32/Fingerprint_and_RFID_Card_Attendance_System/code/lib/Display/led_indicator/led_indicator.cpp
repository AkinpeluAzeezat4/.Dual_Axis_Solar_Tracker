#include <Arduino.h>
#include "led_indicator.h"
#include "Pins/Pins.h"

namespace led_indicator
{
  static uint8_t pin = Pins::UNUSED_PIN;
  static Mode currentMode = OFF;

  static const int pwmChannel = 0;
  static const int pwmFreq = 5000;
  static const int pwmResolution = 8;

  static int brightness = 0;
  static bool rising = true;
  static unsigned long lastUpdate = 0;
  static bool ledState = false;

  void begin(uint8_t ledPin)
  {
    pin = ledPin;

    if (!Pins::valid(pin))
      return;

    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(pin, pwmChannel);
    ledcWrite(pwmChannel, 0);

    currentMode = OFF;
    brightness = 0;
    rising = true;
    ledState = false;
    lastUpdate = millis();
  }

  void update()
  {
    if (!Pins::valid(pin))
      return;

    unsigned long now = millis();

    if (currentMode == OFF)
    {
      ledcWrite(pwmChannel, 0);
      return;
    }

    if (currentMode == ON)
    {
      ledcWrite(pwmChannel, 255);
      return;
    }

    if (currentMode == BREATHING)
    {
      if (now - lastUpdate < 8)
        return;

      lastUpdate = now;

      brightness += rising ? 4 : -4;

      if (brightness >= 255)
      {
        brightness = 255;
        rising = false;
      }

      if (brightness <= 0)
      {
        brightness = 0;
        rising = true;
      }

      ledcWrite(pwmChannel, brightness);
      return;
    }

    unsigned long interval = currentMode == BLINK_FAST ? 150 : 700;

    if (now - lastUpdate >= interval)
    {
      lastUpdate = now;
      ledState = !ledState;
      ledcWrite(pwmChannel, ledState ? 255 : 0);
    }
  }

  void setMode(Mode mode)
  {
    currentMode = mode;
    lastUpdate = millis();

    if (!Pins::valid(pin))
      return;

    if (mode == BREATHING)
    {
      brightness = 0;
      rising = true;
      ledcWrite(pwmChannel, 0);
    }

    if (mode == BLINK_FAST || mode == BLINK_SLOW)
    {
      ledState = false;
      ledcWrite(pwmChannel, 0);
    }

    if (mode == OFF)
      ledcWrite(pwmChannel, 0);

    if (mode == ON)
      ledcWrite(pwmChannel, 255);
  }

  Mode getMode()
  {
    return currentMode;
  }

  void off()
  {
    setMode(OFF);
  }

  void on()
  {
    setMode(ON);
  }
}
