#include <Arduino.h>
#include "robot_control.h"
#include "yahboom_motor/yahboom_motor.h"
#include "ultrasonic/ultrasonic.h"
#include "servo/servo.h"
#include "battery_level/battery_level.h"

namespace robot_control
{
  static Mode currentMode = AUTO_AVOIDANCE;

  static unsigned long lastDecision = 0;
  static unsigned long actionUntil = 0;

  static int16_t manualLeft = 0;
  static int16_t manualRight = 0;

  static uint8_t scanState = 0;
  static float leftDistance = 999.0f;
  static float rightDistance = 999.0f;

  static void driveTank(int16_t left, int16_t right)
  {
    yahboom_motor::setSpeed(left, left, right, right);
  }

  static void autoAvoidance()
  {
    if (battery_level::isLow())
    {
      yahboom_motor::stop();
      return;
    }

    if (millis() < actionUntil)
      return;

    float frontDistance = ultrasonic::getDistanceCm();

    if (frontDistance > 30 && frontDistance < 900)
    {
      servo::setAngle(90);
      yahboom_motor::forward(350);
      scanState = 0;
      return;
    }

    yahboom_motor::stop();

    if (millis() - lastDecision < 350)
      return;
    lastDecision = millis();

    if (scanState == 0)
    {
      servo::setAngle(150);
      scanState = 1;
      return;
    }

    if (scanState == 1)
    {
      leftDistance = ultrasonic::getDistanceCm();
      servo::setAngle(30);
      scanState = 2;
      return;
    }

    if (scanState == 2)
    {
      rightDistance = ultrasonic::getDistanceCm();
      servo::setAngle(90);

      if (leftDistance >= rightDistance)
      {
        yahboom_motor::rotateLeft(350);
      }
      else
      {
        yahboom_motor::rotateRight(350);
      }

      actionUntil = millis() + 550;
      scanState = 0;
    }
  }

  void begin()
  {
    currentMode = AUTO_AVOIDANCE;
    lastDecision = millis();
    actionUntil = 0;
    scanState = 0;
    servo::center();
    yahboom_motor::stop();
  }

  void update()
  {
    if (currentMode == IDLE)
    {
      yahboom_motor::stop();
      return;
    }

    if (currentMode == MANUAL)
    {
      driveTank(manualLeft, manualRight);
      return;
    }

    autoAvoidance();
  }

  void setMode(Mode mode)
  {
    currentMode = mode;

    if (mode == IDLE)
    {
      yahboom_motor::stop();
    }
  }

  void setManualSpeed(int16_t left, int16_t right)
  {
    manualLeft = left;
    manualRight = right;
    currentMode = MANUAL;
  }

  Mode getMode()
  {
    return currentMode;
  }
}
