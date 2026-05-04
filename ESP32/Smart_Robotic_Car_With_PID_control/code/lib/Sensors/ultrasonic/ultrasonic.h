#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

namespace ultrasonic
{
    void begin();
    void update();

    float getDistanceCm();
    unsigned long getLastValidMs();
    bool isObstacle(uint16_t thresholdCm);
    bool hasFreshReading(uint16_t maxAgeMs = 250);
}

#endif
