#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>

namespace motor_driver
{
  enum Mode
  {
    AUTO_TRACK,
    MANUAL_AZIMUTH,
    MANUAL_ELEVATION,
    CALIBRATE_DEADBAND,
    CALIBRATE_NIGHT
  };

  void begin();
  void update();

  void azimuth(int speed);
  void elevation(int speed);
  void stopAll();

  Mode getMode();
  const char *getModeName();

  bool isMovingLeft();
  bool isMovingRight();
  bool isMovingUp();
  bool isMovingDown();
}

#endif
