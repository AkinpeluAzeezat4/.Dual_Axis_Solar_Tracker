#ifndef GYROSCOPE_H
#define GYROSCOPE_H

namespace gyroscope
{
  struct Data
  {
    bool ready = false;
    bool motionInterrupt = false;

    float ax = 0.0f;
    float ay = 0.0f;
    float az = 0.0f;

    float gx = 0.0f;
    float gy = 0.0f;
    float gz = 0.0f;

    float temperature = 0.0f;
  };

  void begin();
  void update();

  Data getData();
  bool isReady();
  bool interruptDetected();
}

#endif
