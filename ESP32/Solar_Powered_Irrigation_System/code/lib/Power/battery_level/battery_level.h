#pragma once
#include <Arduino.h>

namespace battery_level
{
    void begin();
    void update();

    float getVoltage();
    uint8_t getPercentage();
    bool isLow();
}
