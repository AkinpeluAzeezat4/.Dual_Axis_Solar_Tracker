#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "mpu6050_sensor.h"
#include "Pins.h"

namespace mpu6050_sensor
{
  static const uint8_t MPU_ADDR = 0x68;
  static SensorData data = {false, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static unsigned long lastUpdate = 0;
  static const unsigned long updateInterval = 20;

  static bool writeRegister(uint8_t reg, uint8_t value)
  {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission(true) == 0;
  }

  static bool readRaw(int16_t &ax, int16_t &ay, int16_t &az, int16_t &temp, int16_t &gx, int16_t &gy, int16_t &gz)
  {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);

    if (Wire.endTransmission(false) != 0)
    {
      return false;
    }

    if (Wire.requestFrom(MPU_ADDR, (uint8_t)14, (uint8_t)true) != 14)
    {
      return false;
    }

    ax = (Wire.read() << 8) | Wire.read();
    ay = (Wire.read() << 8) | Wire.read();
    az = (Wire.read() << 8) | Wire.read();
    temp = (Wire.read() << 8) | Wire.read();
    gx = (Wire.read() << 8) | Wire.read();
    gy = (Wire.read() << 8) | Wire.read();
    gz = (Wire.read() << 8) | Wire.read();

    return true;
  }

  static void readSensor()
  {
    int16_t rax, ray, raz, rtemp, rgx, rgy, rgz;

    if (!readRaw(rax, ray, raz, rtemp, rgx, rgy, rgz))
    {
      data.ready = false;
      return;
    }

    data.ready = true;

    data.ax = rax / 16384.0f;
    data.ay = ray / 16384.0f;
    data.az = raz / 16384.0f;

    data.gx = rgx / 131.0f;
    data.gy = rgy / 131.0f;
    data.gz = rgz / 131.0f;

    data.temperature = (rtemp / 340.0f) + 36.53f;

    data.pitch = atan2f(data.ay, sqrtf(data.ax * data.ax + data.az * data.az)) * 57.2957795f;
    data.roll = atan2f(-data.ax, data.az) * 57.2957795f;
  }

  void begin()
  {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
    Wire.setClock(400000);
    delay(100);

    Wire.beginTransmission(MPU_ADDR);
    data.ready = Wire.endTransmission() == 0;

    if (data.ready)
    {
      bool ok = true;

      ok &= writeRegister(0x6B, 0x00);
      ok &= writeRegister(0x1A, 0x03);
      ok &= writeRegister(0x1B, 0x00);
      ok &= writeRegister(0x1C, 0x00);

      data.ready = ok;

      if (data.ready)
      {
        readSensor();
        lastUpdate = millis();
      }
    }
  }

  void update()
  {
    unsigned long now = millis();

    if (now - lastUpdate < updateInterval)
    {
      return;
    }

    lastUpdate = now;
    readSensor();
  }

  bool isReady()
  {
    return data.ready;
  }

  SensorData getData()
  {
    return data;
  }
}
