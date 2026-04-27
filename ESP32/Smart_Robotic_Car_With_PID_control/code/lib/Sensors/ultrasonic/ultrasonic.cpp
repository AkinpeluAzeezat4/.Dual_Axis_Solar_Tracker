#include <Arduino.h>
#include "ultrasonic.h"
#include "Pins.h"

namespace ultrasonic
{
  static float distanceCm = 999.0f;
  static unsigned long lastRead = 0;

  static float readDistance()
  {
    digitalWrite(Pins::ULTRASONIC_TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(Pins::ULTRASONIC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(Pins::ULTRASONIC_TRIG, LOW);

    unsigned long duration = pulseIn(Pins::ULTRASONIC_ECHO, HIGH, 25000);

    if (duration == 0)
      return 999.0f;

    return duration * 0.0343f / 2.0f;
  }

  void begin()
  {
    pinMode(Pins::ULTRASONIC_TRIG, OUTPUT);
    pinMode(Pins::ULTRASONIC_ECHO, INPUT);
    digitalWrite(Pins::ULTRASONIC_TRIG, LOW);
  }

  void update()
  {
    if (millis() - lastRead < 80)
      return;

    lastRead = millis();
    distanceCm = readDistance();
  }

  float getDistanceCm()
  {
    return distanceCm;
  }

  bool isObstacle(uint16_t thresholdCm)
  {
    return distanceCm > 0 && distanceCm <= thresholdCm;
  }
}
