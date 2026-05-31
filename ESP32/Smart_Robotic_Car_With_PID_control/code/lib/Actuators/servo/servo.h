#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>

namespace servo
{
    void begin();
    void update();

    void setAngle(uint8_t angle);
    void center();
    uint8_t getAngle();
}

#endif
