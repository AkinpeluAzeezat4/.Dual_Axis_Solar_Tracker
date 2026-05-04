#ifndef SUN_SENSOR_H
#define SUN_SENSOR_H

#include <Arduino.h>

namespace sun_sensor
{
  struct SensorData
  {
    int topLeft;
    int topRight;
    int bottomLeft;
    int bottomRight;
    int night;

    int leftAverage;
    int rightAverage;
    int topAverage;
    int bottomAverage;

    int horizontalError;
    int verticalError;

    int strongestIndex;
    bool isDark;
  };

  void begin();
  void update();

  SensorData getData();

  int getDeadband();

  int getNightThreshold();
  void setNightThreshold(int value);

  void setDeadbandOffset(int value);
  int getDeadbandOffset();
}

#endif
