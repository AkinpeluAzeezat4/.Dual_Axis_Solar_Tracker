#include <Arduino.h>
#include "led_indicator.h"
#include "Pins.h"
#include "sun_sensor/sun_sensor.h"
#include "motor_driver/motor_driver.h"
#include "error_handling/error_handling.h"

namespace led_indicator
{
  int ledPin = Pins::LED_STATUS;

  const int pwmChannel = 0;
  const int pwmFreq = 10000;
  const int pwmResolution = 8;

  unsigned long lastBlink = 0;
  bool statusState = false;

  int brightness = 0;
  bool rising = true;

  unsigned long lastUpdate = 0;
  const int stepTime = 1;

  int pulseCount = 0;

  bool inPause = false;
  unsigned long pauseStart = 0;
  const int pauseDuration = 1000;

  void begin(uint8_t pin)
  {
    ledPin = pin;

    pinMode(Pins::LED_LEFT, OUTPUT);
    pinMode(Pins::LED_RIGHT, OUTPUT);
    pinMode(Pins::LED_UP, OUTPUT);
    pinMode(Pins::LED_DOWN, OUTPUT);
    pinMode(Pins::LED_STATUS, OUTPUT);

    digitalWrite(Pins::LED_LEFT, LOW);
    digitalWrite(Pins::LED_RIGHT, LOW);
    digitalWrite(Pins::LED_UP, LOW);
    digitalWrite(Pins::LED_DOWN, LOW);
    digitalWrite(Pins::LED_STATUS, LOW);

    ledcSetup(pwmChannel, pwmFreq, pwmResolution);
    ledcAttachPin(ledPin, pwmChannel);
  }

  void update()
  {
    unsigned long now = millis();

    sun_sensor::SensorData data = sun_sensor::getData();

    if (data.isDark)
    {
      digitalWrite(Pins::LED_LEFT, LOW);
      digitalWrite(Pins::LED_RIGHT, LOW);
      digitalWrite(Pins::LED_UP, LOW);
      digitalWrite(Pins::LED_DOWN, LOW);
    }
    else if (motor_driver::getMode() == motor_driver::AUTO_TRACK)
    {
      int deadband = sun_sensor::getDeadband();

      digitalWrite(Pins::LED_LEFT, data.horizontalError > deadband);
      digitalWrite(Pins::LED_RIGHT, data.horizontalError < -deadband);
      digitalWrite(Pins::LED_UP, data.verticalError > deadband);
      digitalWrite(Pins::LED_DOWN, data.verticalError < -deadband);
    }
    else
    {
      digitalWrite(Pins::LED_LEFT, motor_driver::isMovingLeft());
      digitalWrite(Pins::LED_RIGHT, motor_driver::isMovingRight());
      digitalWrite(Pins::LED_UP, motor_driver::isMovingUp());
      digitalWrite(Pins::LED_DOWN, motor_driver::isMovingDown());
    }

    if (error_handling::hasError())
    {
      int interval = 120;

      if (now - lastBlink >= interval)
      {
        lastBlink = now;
        statusState = !statusState;
        ledcWrite(pwmChannel, statusState ? 255 : 0);
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
}
