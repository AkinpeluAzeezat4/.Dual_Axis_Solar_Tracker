#include <Arduino.h>
#include <math.h>
#include "robot_control.h"
#include "yahboom_motor/yahboom_motor.h"
#include "ultrasonic/ultrasonic.h"
#include "servo/servo.h"
#include "battery_level/battery_level.h"
#include "gyroscope/gyroscope.h"

namespace robot_control
{
  static Mode currentMode = AUTO_AVOIDANCE;
  static AutoState autoState = DRIVE_FRONT;

  static const uint8_t SERVO_CENTER = 90;
  static const uint8_t SERVO_LEFT = 150;
  static const uint8_t SERVO_RIGHT = 30;

  static const float emergencyDistanceCm = 12.0f;
  static const float turnDecisionDistanceCm = 35.0f;
  static const float slowDistanceCm = 100.0f;
  static const float clearDistanceCm = 45.0f;
  static const float invalidDistanceCm = 900.0f;

  static const int16_t crawlSpeed = 110;
  static const int16_t minDriveSpeed = 150;
  static const int16_t maxDriveSpeed = 430;

  static const int16_t turnInnerSpeed = 90;
  static const int16_t turnMinOuterSpeed = 190;
  static const int16_t turnMaxOuterSpeed = 390;

  static const float turnAngleDeg = 60.0f;

  static const unsigned long scanPrepareTime = 150;
  static const unsigned long servoSettleTime = 420;
  static const unsigned long centerSettleTime = 180;
  static const unsigned long maxTurnTime = 1500;

  static unsigned long stateStart = 0;
  static unsigned long lastDriveUpdate = 0;
  static unsigned long lastTurnUpdate = 0;

  static int16_t manualLeft = 0;
  static int16_t manualRight = 0;

  static int16_t targetLeftSpeed = 0;
  static int16_t targetRightSpeed = 0;
  static int16_t currentLeftSpeed = 0;
  static int16_t currentRightSpeed = 0;

  static float frontDistance = 999.0f;
  static float leftDistance = 999.0f;
  static float rightDistance = 999.0f;

  static float speedErrorLast = 0.0f;
  static float speedErrorIntegral = 0.0f;
  static float distancePidOutput = 0.0f;
  static float turnPidOutput = 0.0f;

  static float turnTargetYaw = 0.0f;
  static int8_t turnDirection = 1;

  static float turnError = 0.0f;
  static float turnErrorLast = 0.0f;
  static float turnErrorIntegral = 0.0f;

  static int16_t clampMotor(int16_t value)
  {
    if (value > 1000)
      return 1000;
    if (value < -1000)
      return -1000;
    return value;
  }

  static bool validDistance(float distance)
  {
    return distance > 0.0f && distance < invalidDistanceCm;
  }

  static float normalizeAngle(float angle)
  {
    while (angle > 180.0f)
      angle -= 360.0f;
    while (angle < -180.0f)
      angle += 360.0f;
    return angle;
  }

  static void changeState(AutoState next)
  {
    autoState = next;
    stateStart = millis();
  }

  static void driveTank(int16_t left, int16_t right)
  {
    targetLeftSpeed = clampMotor(left);
    targetRightSpeed = clampMotor(right);

    currentLeftSpeed = targetLeftSpeed;
    currentRightSpeed = targetRightSpeed;

    yahboom_motor::setPWM(
        currentLeftSpeed,
        currentLeftSpeed,
        currentRightSpeed,
        currentRightSpeed);
  }

  static void crawlForward()
  {
    driveTank(crawlSpeed, crawlSpeed);
  }

  static void stopMotor()
  {
    targetLeftSpeed = 0;
    targetRightSpeed = 0;
    currentLeftSpeed = 0;
    currentRightSpeed = 0;
    yahboom_motor::stop();
  }

  static bool batteryIsSafe()
  {
    float voltage = battery_level::getVoltage();

    if (voltage < 1.0f)
    {
      return true;
    }

    return !battery_level::isLow();
  }

  static int16_t calculateApproachSpeed(float distance)
  {
    if (!validDistance(distance))
      return crawlSpeed;
    if (distance <= turnDecisionDistanceCm)
      return crawlSpeed;
    if (distance >= slowDistanceCm)
      return maxDriveSpeed;

    unsigned long now = millis();
    float dt = 0.02f;

    if (lastDriveUpdate > 0)
    {
      dt = (now - lastDriveUpdate) / 1000.0f;
      if (dt <= 0.0f || dt > 0.5f)
        dt = 0.02f;
    }

    lastDriveUpdate = now;

    float error = distance - turnDecisionDistanceCm;

    speedErrorIntegral += error * dt;
    speedErrorIntegral = constrain(speedErrorIntegral, -80.0f, 80.0f);

    float derivative = (error - speedErrorLast) / dt;
    speedErrorLast = error;

    float kp = 6.0f;
    float ki = 0.12f;
    float kd = 0.35f;

    float output = (kp * error) + (ki * speedErrorIntegral) + (kd * derivative);

    distancePidOutput = constrain(output, minDriveSpeed, maxDriveSpeed);

    return (int16_t)distancePidOutput;
  }

  static void startArcTurn(int8_t direction)
  {
    gyroscope::Data gyro = gyroscope::getData();

    turnDirection = direction;

    if (turnDirection > 0)
    {
      turnTargetYaw = normalizeAngle(gyro.yaw + turnAngleDeg);
      changeState(TURN_RIGHT);
    }
    else
    {
      turnTargetYaw = normalizeAngle(gyro.yaw - turnAngleDeg);
      changeState(TURN_LEFT);
    }

    turnError = 0.0f;
    turnErrorLast = 0.0f;
    turnErrorIntegral = 0.0f;
    lastTurnUpdate = millis();

    servo::setAngle(SERVO_CENTER);
  }

  static bool updateArcTurn()
  {
    gyroscope::Data gyro = gyroscope::getData();

    if (turnDirection > 0)
    {
      turnError = normalizeAngle(turnTargetYaw - gyro.yaw);
    }
    else
    {
      turnError = normalizeAngle(gyro.yaw - turnTargetYaw);
    }

    unsigned long now = millis();
    float dt = (now - lastTurnUpdate) / 1000.0f;

    if (dt <= 0.0f || dt > 0.5f)
      dt = 0.02f;

    lastTurnUpdate = now;

    turnErrorIntegral += turnError * dt;
    turnErrorIntegral = constrain(turnErrorIntegral, -50.0f, 50.0f);

    float derivative = (turnError - turnErrorLast) / dt;
    turnErrorLast = turnError;

    float kp = 4.5f;
    float ki = 0.02f;
    float kd = 0.25f;

    float output = (kp * turnError) + (ki * turnErrorIntegral) + (kd * derivative);

    turnPidOutput = output;

    int16_t outerSpeed = (int16_t)constrain(fabsf(output), turnMinOuterSpeed, turnMaxOuterSpeed);

    if (turnDirection > 0)
    {
      driveTank(outerSpeed, turnInnerSpeed);
    }
    else
    {
      driveTank(turnInnerSpeed, outerSpeed);
    }

    if (fabsf(turnError) <= 5.0f)
    {
      driveTank(crawlSpeed, crawlSpeed);
      return true;
    }

    if (millis() - stateStart >= maxTurnTime)
    {
      driveTank(crawlSpeed, crawlSpeed);
      return true;
    }

    return false;
  }

  static void autoDrive()
  {
    if (!batteryIsSafe())
    {
      stopMotor();
      servo::setAngle(SERVO_CENTER);
      return;
    }

    frontDistance = ultrasonic::getDistanceCm();

    if (validDistance(frontDistance) && frontDistance <= emergencyDistanceCm)
    {
      stopMotor();
      servo::setAngle(SERVO_CENTER);
      return;
    }

    if (autoState == DRIVE_FRONT)
    {
      servo::setAngle(SERVO_CENTER);

      if (!validDistance(frontDistance))
      {
        crawlForward();
        return;
      }

      if (frontDistance <= turnDecisionDistanceCm)
      {
        crawlForward();
        changeState(PREPARE_SCAN);
        return;
      }

      int16_t speed = calculateApproachSpeed(frontDistance);
      driveTank(speed, speed);
      return;
    }

    if (autoState == PREPARE_SCAN)
    {
      crawlForward();
      servo::setAngle(SERVO_CENTER);

      if (millis() - stateStart >= scanPrepareTime)
      {
        servo::setAngle(SERVO_LEFT);
        changeState(LOOK_LEFT);
      }

      return;
    }

    if (autoState == LOOK_LEFT)
    {
      crawlForward();

      if (millis() - stateStart >= servoSettleTime)
      {
        leftDistance = ultrasonic::getDistanceCm();
        changeState(READ_LEFT);
      }

      return;
    }

    if (autoState == READ_LEFT)
    {
      crawlForward();
      servo::setAngle(SERVO_CENTER);
      changeState(RETURN_CENTER_AFTER_LEFT);
      return;
    }

    if (autoState == RETURN_CENTER_AFTER_LEFT)
    {
      crawlForward();

      if (millis() - stateStart >= centerSettleTime)
      {
        servo::setAngle(SERVO_RIGHT);
        changeState(LOOK_RIGHT);
      }

      return;
    }

    if (autoState == LOOK_RIGHT)
    {
      crawlForward();

      if (millis() - stateStart >= servoSettleTime)
      {
        rightDistance = ultrasonic::getDistanceCm();
        changeState(READ_RIGHT);
      }

      return;
    }

    if (autoState == READ_RIGHT)
    {
      crawlForward();
      servo::setAngle(SERVO_CENTER);
      changeState(RETURN_CENTER_AFTER_RIGHT);
      return;
    }

    if (autoState == RETURN_CENTER_AFTER_RIGHT)
    {
      crawlForward();

      if (millis() - stateStart >= centerSettleTime)
      {
        changeState(DECIDE_TURN);
      }

      return;
    }

    if (autoState == DECIDE_TURN)
    {
      crawlForward();
      servo::setAngle(SERVO_CENTER);

      bool leftClear = validDistance(leftDistance) && leftDistance >= clearDistanceCm;
      bool rightClear = validDistance(rightDistance) && rightDistance >= clearDistanceCm;

      if (rightClear && rightDistance >= leftDistance)
      {
        startArcTurn(1);
        return;
      }

      if (leftClear)
      {
        startArcTurn(-1);
        return;
      }

      if (rightClear)
      {
        startArcTurn(1);
        return;
      }

      startArcTurn(1);
      return;
    }

    if (autoState == TURN_RIGHT || autoState == TURN_LEFT)
    {
      servo::setAngle(SERVO_CENTER);

      if (updateArcTurn())
      {
        speedErrorIntegral = 0.0f;
        speedErrorLast = 0.0f;
        changeState(DRIVE_FRONT);
      }

      return;
    }
  }

  void begin()
  {
    currentMode = AUTO_AVOIDANCE;
    autoState = DRIVE_FRONT;

    manualLeft = 0;
    manualRight = 0;

    targetLeftSpeed = 0;
    targetRightSpeed = 0;
    currentLeftSpeed = 0;
    currentRightSpeed = 0;

    frontDistance = 999.0f;
    leftDistance = 999.0f;
    rightDistance = 999.0f;

    speedErrorLast = 0.0f;
    speedErrorIntegral = 0.0f;

    turnTargetYaw = 0.0f;
    turnDirection = 1;
    turnError = 0.0f;
    turnErrorLast = 0.0f;
    turnErrorIntegral = 0.0f;

    servo::setAngle(SERVO_CENTER);
    stopMotor();

    stateStart = millis();
  }

  void update()
  {
    if (currentMode == IDLE)
    {
      stopMotor();
      servo::setAngle(SERVO_CENTER);
      return;
    }

    if (currentMode == MANUAL)
    {
      servo::setAngle(SERVO_CENTER);
      driveTank(manualLeft, manualRight);
      return;
    }

    autoDrive();
  }

  void setMode(Mode mode)
  {
    currentMode = mode;

    if (mode == IDLE)
    {
      stopMotor();
    }

    if (mode == AUTO_AVOIDANCE)
    {
      autoState = DRIVE_FRONT;
      servo::setAngle(SERVO_CENTER);
      speedErrorIntegral = 0.0f;
      speedErrorLast = 0.0f;
    }
  }

  void setManualSpeed(int16_t left, int16_t right)
  {
    manualLeft = clampMotor(left);
    manualRight = clampMotor(right);
    currentMode = MANUAL;
  }

  void stop()
  {
    currentMode = IDLE;
    stopMotor();
    servo::setAngle(SERVO_CENTER);
  }

  Mode getMode()
  {
    return currentMode;
  }

  AutoState getAutoState()
  {
    return autoState;
  }

  Status getStatus()
  {
    Status status;

    gyroscope::Data gyro = gyroscope::getData();

    status.mode = currentMode;
    status.autoState = autoState;

    status.frontDistance = frontDistance;
    status.leftDistance = leftDistance;
    status.rightDistance = rightDistance;

    status.targetLeftSpeed = targetLeftSpeed;
    status.targetRightSpeed = targetRightSpeed;
    status.currentLeftSpeed = currentLeftSpeed;
    status.currentRightSpeed = currentRightSpeed;

    status.yaw = gyro.yaw;
    status.targetYaw = turnTargetYaw;
    status.turnError = turnError;

    status.distancePidOutput = distancePidOutput;
    status.turnPidOutput = turnPidOutput;

    status.servoAngle = servo::getAngle();
    return status;
  }

  float getFrontDistance()
  {
    return frontDistance;
  }

  float getLeftDistance()
  {
    return leftDistance;
  }

  float getRightDistance()
  {
    return rightDistance;
  }

  int16_t getLeftSpeed()
  {
    return currentLeftSpeed;
  }

  int16_t getRightSpeed()
  {
    return currentRightSpeed;
  }
}
