#include <Arduino.h>
#include <ESP32Servo.h>
#include "servo.h"
#include "Pins.h"

namespace servo
{
    static Servo neckServo;
    static uint8_t currentAngle = 90;
    static uint8_t targetAngle = 90;
    static unsigned long lastMove = 0;
    static const unsigned long moveInterval = 20;
    static const uint8_t minAngle = 30;
    static const uint8_t maxAngle = 150;
    static const uint8_t stepSize = 2;

    void begin()
    {
        neckServo.setPeriodHertz(50);
        neckServo.attach(Pins::SERVO_SIGNAL, 500, 2400);
        currentAngle = 90;
        targetAngle = 90;
        neckServo.write(currentAngle);
    }

    void update()
    {
        if (millis() - lastMove < moveInterval) return;
        lastMove = millis();

        if (currentAngle == targetAngle) return;

        if (currentAngle < targetAngle)
        {
            uint8_t nextAngle = currentAngle + stepSize;
            currentAngle = nextAngle > targetAngle ? targetAngle : nextAngle;
        }
        else
        {
            int nextAngle = currentAngle - stepSize;
            currentAngle = nextAngle < targetAngle ? targetAngle : nextAngle;
        }

        neckServo.write(currentAngle);
    }

    void setAngle(uint8_t angle)
    {
        targetAngle = constrain(angle, minAngle, maxAngle);
    }

    void center()
    {
        setAngle(90);
    }

    uint8_t getAngle()
    {
        return currentAngle;
    }

    uint8_t getTargetAngle()
    {
        return targetAngle;
    }

    bool isAtTarget()
    {
        return currentAngle == targetAngle;
    }
}
