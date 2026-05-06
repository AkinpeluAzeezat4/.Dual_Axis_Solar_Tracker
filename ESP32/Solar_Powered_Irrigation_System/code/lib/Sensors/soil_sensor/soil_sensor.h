#pragma once
#include <Arduino.h>

namespace soil_sensor
{
    void begin();
    void update();

    int getRaw();
    uint8_t getMoisturePercent();
    bool isDry();
}
