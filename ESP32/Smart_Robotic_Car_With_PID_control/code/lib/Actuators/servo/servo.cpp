#include <Arduino.h>
#include <ESP32Servo.h>
#include "servo.h"
#include "Pins.h"

namespace servo
{
    static Servo neckServo;
    static uint8_t currentAngle = 90;

    void begin()
    {
        neckServo.setPeriodHertz(50);
        neckServo.attach(Pins::SERVO_SIGNAL, 500, 2400);
        center();
    }

    void update()
    {
    }

    void setAngle(uint8_t angle)
    {
        currentAngle = constrain(angle, 30, 150);
        neckServo.write(currentAngle);
    }

    void center()
    {
        setAngle(90);
    }

    uint8_t getAngle()
    {
        return currentAngle;
    }
}
