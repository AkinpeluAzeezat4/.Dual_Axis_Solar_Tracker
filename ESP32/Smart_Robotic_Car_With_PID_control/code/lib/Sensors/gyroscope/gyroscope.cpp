#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "gyroscope.h"
#include "Pins.h"

namespace gyroscope
{
  static Adafruit_MPU6050 mpu;
  static Data data;
  static volatile bool intFlag = false;
  static unsigned long lastRead = 0;

  static void IRAM_ATTR onMpuInterrupt()
  {
    intFlag = true;
  }

  void begin()
  {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);

    data.ready = mpu.begin(0x68, &Wire);
    if (!data.ready)
      return;

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    pinMode(Pins::MPU_INT, INPUT);
    attachInterrupt(digitalPinToInterrupt(Pins::MPU_INT), onMpuInterrupt, RISING);
  }

  void update()
  {
    if (!data.ready)
      return;
    if (!intFlag && millis() - lastRead < 50)
      return;

    lastRead = millis();
    data.motionInterrupt = intFlag;
    intFlag = false;

    sensors_event_t a;
    sensors_event_t g;
    sensors_event_t temp;

    mpu.getEvent(&a, &g, &temp);

    data.ax = a.acceleration.x;
    data.ay = a.acceleration.y;
    data.az = a.acceleration.z;

    data.gx = g.gyro.x;
    data.gy = g.gyro.y;
    data.gz = g.gyro.z;

    data.temperature = temp.temperature;
  }

  Data getData()
  {
    return data;
  }

  bool isReady()
  {
    return data.ready;
  }

  bool interruptDetected()
  {
    bool state = data.motionInterrupt;
    data.motionInterrupt = false;
    return state;
  }
}
