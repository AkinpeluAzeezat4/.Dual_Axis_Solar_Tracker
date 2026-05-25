#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>

namespace voltage_sensor
{
  void begin();
  void update();

  float getVoltageRMS();
  float getAdcVoltageRMS();
  int getRawADC();
  bool hasFreshReading();

  void setCalibration(float calibration);
  float getCalibration();
}

#endif
