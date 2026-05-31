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
  static const uint8_t SERVO_RIGHT = 0;
  static const uint8_t SERVO_LEFT = 180;

  static const float reverseDistanceCm = 8.0f;
  static const float scanDistanceCm = 15.0f;
  static const float slowDistanceCm = 35.0f;
  static const float invalidDistanceCm = 900.0f;

  static const int16_t reverseSpeed = 75;
  static const int16_t minDriveSpeed = 60;
  static const int16_t maxDriveSpeed = 300;

  static const int16_t turnMinSpeed = 120;
  static const int16_t turnMaxSpeed = 230;

  static const float turnAngleDeg = 35.0f;

  static const unsigned long stopBeforeScanTime = 250;
  static const unsigned long reverseTime = 800;
  static const unsigned long servoSettleTime = 500;
  static const unsigned long sampleInterval = 60;
  static const unsigned long centerSettleTime = 250;
  static const unsigned long maxTurnTime = 1050;

  static const uint8_t sampleTarget = 4;

  static unsigned long stateStart = 0;
  static unsigned long lastDriveUpdate = 0;
  static unsigned long lastTurnUpdate = 0;
  static unsigned long lastSampleTime = 0;

  static int16_t manualLeft = 0;
  static int16_t manualRight = 0;

  static int16_t targetLeftSpeed = 0;
  static int16_t targetRightSpeed = 0;
  static int16_t currentLeftSpeed = 0;
  static int16_t currentRightSpeed = 0;

  static float frontDistance = 999.0f;
  static float leftDistance = 999.0f;
  static float rightDistance = 999.0f;

  static float sampleSum = 0.0f;
  static uint8_t sampleCount = 0;
  static uint8_t sampleAttempts = 0;

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

  static void resetSample()
  {
    sampleSum = 0.0f;
    sampleCount = 0;
    sampleAttempts = 0;
    lastSampleTime = 0;
  }

  static bool collectDistanceSample(float &result)
  {
    if (millis() - lastSampleTime < sampleInterval)
      return false;

    lastSampleTime = millis();
    sampleAttempts++;

    float distance = ultrasonic::getDistanceCm();

    if (validDistance(distance))
    {
      sampleSum += distance;
      sampleCount++;
    }

    if (sampleAttempts >= sampleTarget)
    {
      if (sampleCount > 0)
      {
        result = sampleSum / sampleCount;
      }
      else
      {
        result = 999.0f;
      }

      return true;
    }

    return false;
  }

  static void driveTank(int16_t left, int16_t right)
  {
    targetLeftSpeed = clampMotor(left);
    targetRightSpeed = clampMotor(right);

    currentLeftSpeed = targetLeftSpeed;
    currentRightSpeed = targetRightSpeed;

    yahboom_motor::setSpeed(
        currentLeftSpeed,
        currentLeftSpeed,
        currentRightSpeed,
        currentRightSpeed);
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
      return minDriveSpeed;
    if (distance <= scanDistanceCm)
      return 0;
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

    float error = distance - scanDistanceCm;

    speedErrorIntegral += error * dt;
    speedErrorIntegral = constrain(speedErrorIntegral, -50.0f, 50.0f);

    float derivative = (error - speedErrorLast) / dt;
    speedErrorLast = error;

    float kp = 5.0f;
    float ki = 0.08f;
    float kd = 0.20f;

    float output = (kp * error) + (ki * speedErrorIntegral) + (kd * derivative);

    distancePidOutput = constrain(output, minDriveSpeed, maxDriveSpeed);

    return (int16_t)distancePidOutput;
  }

  static void startTurn(int8_t direction)
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
    turnPidOutput = 0.0f;
    lastTurnUpdate = millis();

    servo::center();
  }

  static bool updateTurn()
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
    turnErrorIntegral = constrain(turnErrorIntegral, -35.0f, 35.0f);

    float derivative = (turnError - turnErrorLast) / dt;
    turnErrorLast = turnError;

    float kp = 5.5f;
    float ki = 0.01f;
    float kd = 0.25f;

    float output = (kp * turnError) + (ki * turnErrorIntegral) + (kd * derivative);

    turnPidOutput = output;

    int16_t turnSpeed = (int16_t)constrain(fabsf(output), turnMinSpeed, turnMaxSpeed);

    if (turnDirection > 0)
    {
      driveTank(turnSpeed, -turnSpeed);
    }
    else
    {
      driveTank(-turnSpeed, turnSpeed);
    }

    if (fabsf(turnError) <= 5.0f)
    {
      return true;
    }

    if (millis() - stateStart >= maxTurnTime)
    {
      return true;
    }

    return false;
  }

  static void autoDrive()
  {
    if (!batteryIsSafe())
    {
      stopMotor();
      servo::center();
      return;
    }

    if (autoState == DRIVE_FRONT)
    {
      servo::center();

      frontDistance = ultrasonic::getDistanceCm();

      if (validDistance(frontDistance) && frontDistance <= reverseDistanceCm)
      {
        stopMotor();
        changeState(REVERSE_BACK);
        return;
      }

      if (validDistance(frontDistance) && frontDistance <= scanDistanceCm)
      {
        stopMotor();
        changeState(STOP_FOR_DECISION);
        return;
      }

      int16_t speed = calculateApproachSpeed(frontDistance);
      driveTank(speed, speed);
      return;
    }

    if (autoState == REVERSE_BACK)
    {
      servo::center();
      driveTank(-reverseSpeed, -reverseSpeed);

      if (millis() - stateStart >= reverseTime)
      {
        stopMotor();
        changeState(STOP_FOR_DECISION);
      }

      return;
    }

    if (autoState == STOP_FOR_DECISION)
    {
      stopMotor();
      servo::center();

      if (millis() - stateStart >= stopBeforeScanTime)
      {
        servo::setAngle(SERVO_RIGHT);
        resetSample();
        changeState(LOOK_RIGHT);
      }

      return;
    }

    if (autoState == LOOK_RIGHT)
    {
      stopMotor();
      servo::setAngle(SERVO_RIGHT);

      if (millis() - stateStart >= servoSettleTime)
      {
        resetSample();
        changeState(SAMPLE_RIGHT);
      }

      return;
    }

    if (autoState == SAMPLE_RIGHT)
    {
      stopMotor();
      servo::setAngle(SERVO_RIGHT);

      if (collectDistanceSample(rightDistance))
      {
        servo::setAngle(SERVO_LEFT);
        resetSample();
        changeState(LOOK_LEFT);
      }

      return;
    }

    if (autoState == LOOK_LEFT)
    {
      stopMotor();
      servo::setAngle(SERVO_LEFT);

      if (millis() - stateStart >= servoSettleTime)
      {
        resetSample();
        changeState(SAMPLE_LEFT);
      }

      return;
    }

    if (autoState == SAMPLE_LEFT)
    {
      stopMotor();
      servo::setAngle(SERVO_LEFT);

      if (collectDistanceSample(leftDistance))
      {
        servo::center();
        changeState(RETURN_CENTER);
      }

      return;
    }

    if (autoState == RETURN_CENTER)
    {
      stopMotor();
      servo::center();

      if (millis() - stateStart >= centerSettleTime)
      {
        changeState(DECIDE_TURN);
      }

      return;
    }

    if (autoState == DECIDE_TURN)
    {
      stopMotor();
      servo::center();

      if (rightDistance >= leftDistance)
      {
        startTurn(1);
        return;
      }

      startTurn(-1);
      return;
    }

    if (autoState == TURN_RIGHT || autoState == TURN_LEFT)
    {
      servo::center();

      if (updateTurn())
      {
        stopMotor();
        distancePidOutput = 0.0f;
        turnPidOutput = 0.0f;
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

    sampleSum = 0.0f;
    sampleCount = 0;
    sampleAttempts = 0;

    speedErrorLast = 0.0f;
    speedErrorIntegral = 0.0f;

    distancePidOutput = 0.0f;
    turnPidOutput = 0.0f;

    turnTargetYaw = 0.0f;
    turnDirection = 1;
    turnError = 0.0f;
    turnErrorLast = 0.0f;
    turnErrorIntegral = 0.0f;

    servo::center();
    stopMotor();

    stateStart = millis();
  }

  void update()
  {
    if (currentMode == IDLE)
    {
      stopMotor();
      servo::center();
      return;
    }

    if (currentMode == MANUAL)
    {
      servo::center();
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
      servo::center();
      speedErrorIntegral = 0.0f;
      speedErrorLast = 0.0f;
      distancePidOutput = 0.0f;
      turnPidOutput = 0.0f;
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
    servo::center();
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
