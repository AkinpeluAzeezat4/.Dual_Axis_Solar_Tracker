#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

#include <Arduino.h>

namespace robot_control
{
    enum Mode
    {
        IDLE,
        MANUAL,
        AUTO_AVOIDANCE,
        AUTO_DRIVE = AUTO_AVOIDANCE
    };

    enum AutoState
    {
        DRIVE_FRONT,
        STOP_FOR_DECISION,
        REVERSE_BACK,
        LOOK_RIGHT,
        SAMPLE_RIGHT,
        LOOK_LEFT,
        SAMPLE_LEFT,
        RETURN_CENTER,
        DECIDE_TURN,
        TURN_RIGHT,
        TURN_LEFT,

        DRIVE_FORWARD = DRIVE_FRONT,
        PREPARE_SCAN = STOP_FOR_DECISION,
        STOP_FOR_SCAN = STOP_FOR_DECISION,
        SCAN_RIGHT = LOOK_RIGHT,
        READ_RIGHT = SAMPLE_RIGHT,
        SCAN_LEFT = LOOK_LEFT,
        READ_LEFT = SAMPLE_LEFT,
        RETURN_CENTER_AFTER_RIGHT = RETURN_CENTER,
        RETURN_CENTER_AFTER_LEFT = RETURN_CENTER,
        DECIDE_DIRECTION = DECIDE_TURN
    };

    struct Status
    {
        Mode mode = AUTO_AVOIDANCE;
        AutoState autoState = DRIVE_FRONT;

        float frontDistance = 999.0f;
        float leftDistance = 999.0f;
        float rightDistance = 999.0f;

        int16_t targetLeftSpeed = 0;
        int16_t targetRightSpeed = 0;
        int16_t currentLeftSpeed = 0;
        int16_t currentRightSpeed = 0;

        float yaw = 0.0f;
        float targetYaw = 0.0f;
        float turnError = 0.0f;

        float distancePidOutput = 0.0f;
        float turnPidOutput = 0.0f;

        uint8_t servoAngle = 90;
    };

    void begin();
    void update();

    void setMode(Mode mode);
    void setManualSpeed(int16_t left, int16_t right);
    void stop();

    Mode getMode();
    AutoState getAutoState();
    Status getStatus();

    float getFrontDistance();
    float getLeftDistance();
    float getRightDistance();

    int16_t getLeftSpeed();
    int16_t getRightSpeed();
}

#endif
