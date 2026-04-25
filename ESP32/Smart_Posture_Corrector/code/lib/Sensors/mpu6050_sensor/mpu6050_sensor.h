#pragma once
#include <Arduino.h>

namespace mpu6050_sensor
{
  struct SensorData
  {
    bool ready;
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
    float temperature;
    float pitch;
    float roll;
  };

  void begin();
  void update();

  bool isReady();
  SensorData getData();
}
