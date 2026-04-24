#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <math.h>
#include "Pins.h"
#include "vibration_sensor.h"

namespace vibration_sensor
{
  static Adafruit_ADXL345_Unified adxl = Adafruit_ADXL345_Unified(12345);

  static bool ready = false;

  static float x = 0.0f;
  static float y = 0.0f;
  static float z = 0.0f;

  static float baseMagnitude = 9.81f;
  static float vibrationRms = 0.0f;

  static unsigned long lastUpdate = 0;
  static const unsigned long interval = 100;

  void begin()
  {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);

    ready = adxl.begin(0x53);

    if (ready)
    {
      adxl.setRange(ADXL345_RANGE_16_G);

      sensors_event_t event;
      adxl.getEvent(&event);

      baseMagnitude = sqrt(
          event.acceleration.x * event.acceleration.x +
          event.acceleration.y * event.acceleration.y +
          event.acceleration.z * event.acceleration.z);
    }
  }

  void update()
  {
    if (!ready || millis() - lastUpdate < interval)
    {
      return;
    }

    lastUpdate = millis();

    const uint8_t samples = 20;
    float sumSq = 0.0f;

    for (uint8_t i = 0; i < samples; i++)
    {
      sensors_event_t event;
      adxl.getEvent(&event);

      x = event.acceleration.x / 9.80665f;
      y = event.acceleration.y / 9.80665f;
      z = event.acceleration.z / 9.80665f;

      float mag = sqrt(
          event.acceleration.x * event.acceleration.x +
          event.acceleration.y * event.acceleration.y +
          event.acceleration.z * event.acceleration.z);

      float deltaG = (mag - baseMagnitude) / 9.80665f;

      sumSq += deltaG * deltaG;

      delay(2);
    }

    vibrationRms = sqrt(sumSq / samples);
  }

  float getX()
  {
    return x;
  }

  float getY()
  {
    return y;
  }

  float getZ()
  {
    return z;
  }

  float getVibrationRMS()
  {
    return vibrationRms;
  }

  bool isReady()
  {
    return ready;
  }
}
