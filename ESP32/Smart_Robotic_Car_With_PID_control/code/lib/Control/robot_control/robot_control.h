#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

#include <Arduino.h>

namespace robot_control
{
  enum Mode
  {
    IDLE,
    MANUAL,
    AUTO_AVOIDANCE
  };

  void begin();
  void update();

  void setMode(Mode mode);
  void setManualSpeed(int16_t left, int16_t right);
  Mode getMode();
}

#endif
