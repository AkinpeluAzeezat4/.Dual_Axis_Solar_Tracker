#ifndef YAHBOOM_MOTOR_H
#define YAHBOOM_MOTOR_H

#include <Arduino.h>

namespace yahboom_motor
{
  struct MotorData
  {
    bool ready = false;
    bool configured = false;
    bool hasNewFrame = false;

    long encoderTotal[4] = {0, 0, 0, 0};
    long encoderOffset[4] = {0, 0, 0, 0};

    int16_t targetSpeed[4] = {0, 0, 0, 0};
    int16_t commandedSpeed[4] = {0, 0, 0, 0};

    float measuredSpeed[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    float speed[4] = {0.0f, 0.0f, 0.0f, 0.0f};
  };

  void begin();
  void update();

  void setSpeed(int16_t m1, int16_t m2, int16_t m3, int16_t m4);
  void setPWM(int16_t m1, int16_t m2, int16_t m3, int16_t m4);

  void stop();
  void forward(int16_t speed);
  void backward(int16_t speed);
  void turnLeft(int16_t speed);
  void turnRight(int16_t speed);
  void rotateLeft(int16_t speed);
  void rotateRight(int16_t speed);

  MotorData getData();

  bool isReady();
  bool isConfigured();
}

#endif
