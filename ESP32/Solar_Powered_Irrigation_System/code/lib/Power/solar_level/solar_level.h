#pragma once
#include <Arduino.h>

namespace solar_level
{
    void begin();
    void update();

    float getVoltage();
    bool isPresent();
}
