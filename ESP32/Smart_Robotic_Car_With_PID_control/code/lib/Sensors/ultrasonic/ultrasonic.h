#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

namespace ultrasonic
{
  void begin();
  void update();

  float getDistanceCm();
  bool isObstacle(uint16_t thresholdCm);
}

#endif
