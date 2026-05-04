#include <Arduino.h>
#include "robot_control.h"
#include "pid_controller/pid_controller.h"
#include "yahboom_motor/yahboom_motor.h"
#include "ultrasonic/ultrasonic.h"
#include "servo/servo.h"
#include "battery_level/battery_level.h"
#include "gyroscope/gyroscope.h"

namespace robot_control
{
    static Mode currentMode = AUTO_DRIVE;
    static AutoState autoState = DRIVE_FORWARD;
    static Status status;

    static PIDController distancePid;
    static PIDController turnPid;

    static int16_t manualLeft = 0;
    static int16_t manualRight = 0;
    static int16_t currentLeft = 0;
    static int16_t currentRight = 0;
    static int16_t targetLeft = 0;
    static int16_t targetRight = 0;

    static float rightScanDistance = 999.0f;
    static float leftScanDistance = 999.0f;
    static float targetYaw = 0.0f;
    static float lastYaw = 0.0f;

    static unsigned long stateStart = 0;
    static unsigned long lastControl = 0;

    static const unsigned long controlInterval = 30;
    static const unsigned long scanSettleTime = 650;
    static const unsigned long reverseDuration = 500;
    static const unsigned long fallbackTurnDuration = 850;

    static const float stopDistance = 25.0f;
    static const float slowDistance = 85.0f;
    static const float clearDistance = 45.0f;
    static const float yawTurnAngle = 68.0f;
    static const float yawTolerance = 5.0f;

    static const int16_t maxForwardSpeed = 480;
    static const int16_t minForwardSpeed = 130;
    static const int16_t maxTurnSpeed = 390;
    static const int16_t reverseSpeed = 260;
    static const int16_t speedRampStep = 18;

    static void changeState(AutoState next)
    {
        autoState = next;
        stateStart = millis();
        turnPid.reset();
    }

    static int16_t rampValue(int16_t current, int16_t target)
    {
        if (current < target)
        {
            int16_t next = current + speedRampStep;
            return next > target ? target : next;
        }

        if (current > target)
        {
            int16_t next = current - speedRampStep;
            return next < target ? target : next;
        }

        return current;
    }

    static int16_t clampMotor(int16_t value)
    {
        if (value > 1000) return 1000;
        if (value < -1000) return -1000;
        return value;
    }

    static void setTankTarget(int16_t left, int16_t right)
    {
        targetLeft = clampMotor(left);
        targetRight = clampMotor(right);
    }

    static void applyMotorOutput()
    {
        currentLeft = rampValue(currentLeft, targetLeft);
        currentRight = rampValue(currentRight, targetRight);
        yahboom_motor::setSpeed(currentLeft, currentLeft, currentRight, currentRight);
    }

    static int16_t calculateApproachSpeed(float distance, float dt)
    {
        if (distance <= stopDistance) return 0;
        if (distance >= slowDistance || distance >= 900.0f) return maxForwardSpeed;

        float error = distance - stopDistance;
        float pidSpeed = distancePid.compute(error, dt);
        pidSpeed = constrain(pidSpeed, (float)minForwardSpeed, (float)maxForwardSpeed);
        return (int16_t)pidSpeed;
    }

    static bool yawReached(float error)
    {
        return fabsf(error) <= yawTolerance;
    }

    static void runTurn(bool right, float dt)
    {
        gyroscope::Data gyro = gyroscope::getData();
        lastYaw = gyro.yaw;

        if (!gyro.ready)
        {
            if (right) setTankTarget(maxTurnSpeed, -maxTurnSpeed);
            else setTankTarget(-maxTurnSpeed, maxTurnSpeed);

            if (millis() - stateStart >= fallbackTurnDuration)
            {
                setTankTarget(0, 0);
                servo::center();
                distancePid.reset();
                changeState(DRIVE_FORWARD);
            }
            return;
        }

        float error = targetYaw - gyro.yaw;
        float output = turnPid.compute(error, dt);
        output = constrain(output, -(float)maxTurnSpeed, (float)maxTurnSpeed);

        if (fabsf(output) < 160.0f)
        {
            output = output < 0.0f ? -160.0f : 160.0f;
        }

        setTankTarget((int16_t)(-output), (int16_t)output);

        if (yawReached(error) || millis() - stateStart >= 1400)
        {
            setTankTarget(0, 0);
            servo::center();
            distancePid.reset();
            changeState(DRIVE_FORWARD);
        }
    }

    static void autoDrive(float dt)
    {
        // if (battery_level::isLow())
        // {
        //     setTankTarget(0, 0);
        //     return;
        // }

        float frontDistance = ultrasonic::getDistanceCm();

        if (autoState == DRIVE_FORWARD)
        {
            servo::center();

            if (frontDistance <= stopDistance)
            {
                setTankTarget(0, 0);
                changeState(STOP_FOR_SCAN);
                return;
            }

            int16_t speed = calculateApproachSpeed(frontDistance, dt);
            setTankTarget(speed, speed);
            return;
        }

        if (autoState == STOP_FOR_SCAN)
        {
            setTankTarget(0, 0);
            servo::setAngle(30);

            if (millis() - stateStart >= scanSettleTime && servo::isAtTarget())
            {
                changeState(SCAN_RIGHT);
            }
            return;
        }

        if (autoState == SCAN_RIGHT)
        {
            setTankTarget(0, 0);

            if (millis() - stateStart >= 250)
            {
                rightScanDistance = ultrasonic::getDistanceCm();
                servo::setAngle(150);
                changeState(SCAN_LEFT);
            }
            return;
        }

        if (autoState == SCAN_LEFT)
        {
            setTankTarget(0, 0);

            if (millis() - stateStart >= scanSettleTime && servo::isAtTarget())
            {
                leftScanDistance = ultrasonic::getDistanceCm();
                servo::center();
                changeState(DECIDE_DIRECTION);
            }
            return;
        }

        if (autoState == DECIDE_DIRECTION)
        {
            setTankTarget(0, 0);
            gyroscope::Data gyro = gyroscope::getData();
            lastYaw = gyro.yaw;

            if (rightScanDistance >= clearDistance)
            {
                targetYaw = gyro.yaw - yawTurnAngle;
                changeState(TURN_RIGHT);
                return;
            }

            if (leftScanDistance >= clearDistance)
            {
                targetYaw = gyro.yaw + yawTurnAngle;
                changeState(TURN_LEFT);
                return;
            }

            changeState(REVERSE_BACK);
            return;
        }

        if (autoState == TURN_RIGHT)
        {
            runTurn(true, dt);
            return;
        }

        if (autoState == TURN_LEFT)
        {
            runTurn(false, dt);
            return;
        }

        if (autoState == REVERSE_BACK)
        {
            setTankTarget(-reverseSpeed, -reverseSpeed);

            if (millis() - stateStart >= reverseDuration)
            {
                gyroscope::Data gyro = gyroscope::getData();
                targetYaw = gyro.yaw - yawTurnAngle;
                changeState(TURN_RIGHT);
            }
        }
    }

    static void updateStatus()
    {
        gyroscope::Data gyro = gyroscope::getData();

        status.mode = currentMode;
        status.autoState = autoState;
        status.frontDistance = ultrasonic::getDistanceCm();
        status.rightDistance = rightScanDistance;
        status.leftDistance = leftScanDistance;
        status.targetLeftSpeed = targetLeft;
        status.targetRightSpeed = targetRight;
        status.currentLeftSpeed = currentLeft;
        status.currentRightSpeed = currentRight;
        status.yaw = gyro.yaw;
        status.targetYaw = targetYaw;
        status.turnError = targetYaw - gyro.yaw;
        status.servoAngle = servo::getAngle();
    }

    void begin()
    {
        currentMode = AUTO_DRIVE;
        autoState = DRIVE_FORWARD;
        stateStart = millis();
        lastControl = millis();

        distancePid.setTunings(6.0f, 0.0f, 0.8f);
        distancePid.setOutputLimits(0.0f, maxForwardSpeed);

        turnPid.setTunings(7.0f, 0.0f, 0.4f);
        turnPid.setOutputLimits(-maxTurnSpeed, maxTurnSpeed);

        servo::center();
        setTankTarget(0, 0);
        yahboom_motor::stop();
        updateStatus();
    }

    void update()
    {
        unsigned long now = millis();
        if (now - lastControl < controlInterval)
        {
            updateStatus();
            return;
        }

        float dt = (now - lastControl) / 1000.0f;
        lastControl = now;

        if (currentMode == IDLE)
        {
            setTankTarget(0, 0);
        }
        else if (currentMode == MANUAL)
        {
            setTankTarget(manualLeft, manualRight);
        }
        else
        {
            autoDrive(dt);
        }

        applyMotorOutput();
        updateStatus();
    }

    void setMode(Mode mode)
    {
        currentMode = mode;
        distancePid.reset();
        turnPid.reset();

        if (mode == IDLE)
        {
            setTankTarget(0, 0);
        }

        if (mode == AUTO_DRIVE)
        {
            changeState(DRIVE_FORWARD);
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
        setTankTarget(0, 0);
        yahboom_motor::stop();
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
        return status;
    }
}
