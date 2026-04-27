#include <Arduino.h>
#include <ESP32Servo.h>
#include "servo.h"
#include "Pins.h"

namespace servo
{
  static Servo neckServo;
  static int currentAngle = 90;

  void begin()
  {
    neckServo.setPeriodHertz(50);
    neckServo.attach(Pins::SERVO_PIN, 500, 2400);
    neckServo.write(currentAngle);
  }

  void update()
  {
  }

  void setAngle(int angle)
  {
    currentAngle = constrain(angle, 30, 150);
    neckServo.write(currentAngle);
  }

  int getAngle()
  {
    return currentAngle;
  }

  void center()
  {
    setAngle(90);
  }
}
