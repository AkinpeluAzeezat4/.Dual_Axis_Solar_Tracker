#ifndef VIBRATION_SENSOR_H
#define VIBRATION_SENSOR_H

#include <Arduino.h>

namespace vibration_sensor
{
  void begin();
  void update();

  float getX();
  float getY();
  float getZ();

  float getVibrationRMS();

  bool isReady();
}

#endif
