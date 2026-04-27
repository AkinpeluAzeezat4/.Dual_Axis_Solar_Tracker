#include <Arduino.h>
#include "ultrasonic.h"

namespace ultrasonic
{
  static uint8_t trig = 255;
  static uint8_t echo = 255;

  static float distanceCm = 0.0f;
  static unsigned long lastRead = 0;

  void begin(uint8_t trigPin, uint8_t echoPin)
  {
    trig = trigPin;
    echo = echoPin;

    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);

    digitalWrite(trig, LOW);
  }

  void update()
  {
    if (millis() - lastRead < 500)
    {
      return;
    }

    lastRead = millis();

    digitalWrite(trig, LOW);
    delayMicroseconds(2);

    digitalWrite(trig, HIGH);
    delayMicroseconds(10);

    digitalWrite(trig, LOW);

    unsigned long duration = pulseIn(echo, HIGH, 30000UL);

    if (duration == 0)
    {
      return;
    }

    distanceCm = duration * 0.0343f / 2.0f;
  }

  float getDistanceCm()
  {
    return distanceCm;
  }

  bool isObjectDetected(float thresholdCm)
  {
    return distanceCm > 0 && distanceCm <= thresholdCm;
  }
}
