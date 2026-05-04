#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

#include <Arduino.h>

namespace robot_control
{
    enum Mode
    {
        IDLE,
        MANUAL,
        AUTO_DRIVE
    };

    enum AutoState
    {
        DRIVE_FORWARD,
        STOP_FOR_SCAN,
        SCAN_RIGHT,
        SCAN_LEFT,
        DECIDE_DIRECTION,
        TURN_RIGHT,
        TURN_LEFT,
        REVERSE_BACK
    };

    struct Status
    {
        Mode mode = AUTO_DRIVE;
        AutoState autoState = DRIVE_FORWARD;

        float frontDistance = 999.0f;
        float rightDistance = 999.0f;
        float leftDistance = 999.0f;

        int16_t targetLeftSpeed = 0;
        int16_t targetRightSpeed = 0;
        int16_t currentLeftSpeed = 0;
        int16_t currentRightSpeed = 0;

        float yaw = 0.0f;
        float targetYaw = 0.0f;
        float turnError = 0.0f;
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
}

#endif
