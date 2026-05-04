#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include <stdint.h>

namespace led_indicator
{
    void begin();
    void update();

    void setBatteryLevel(uint8_t percentage);
    void enablePowerBreathing(bool state);
}

#endif
