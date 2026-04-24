#include <Arduino.h>
#include <math.h>
#include "Pins.h"
#include "current_sensor.h"

namespace current_sensor
{
  static const float ADC_REF_V = 3.3f;
  static const int ADC_MAX = 4095;

  static float sensorCalibration = 30.0f;

  static const unsigned long SAMPLE_INTERVAL_MS = 500;

  static float currentA = 0.0f;
  static float voltageRms = 0.0f;
  static int rawAdc = 0;

  static unsigned long lastSample = 0;

  void begin()
  {
    analogReadResolution(12);
    analogSetPinAttenuation(Pins::ZMCT103C_ADC, ADC_11db);
  }

  void update()
  {
    if (millis() - lastSample < SAMPLE_INTERVAL_MS)
    {
      return;
    }

    lastSample = millis();

    const uint16_t samples = 600;
    double sum = 0.0;
    double sumSq = 0.0;

    for (uint16_t i = 0; i < samples; i++)
    {
      rawAdc = analogRead(Pins::ZMCT103C_ADC);

      float v = (rawAdc * ADC_REF_V) / ADC_MAX;

      sum += v;
      sumSq += v * v;

      delayMicroseconds(250);
    }

    double mean = sum / samples;
    double meanSq = sumSq / samples;
    double variance = meanSq - (mean * mean);

    if (variance < 0.0)
    {
      variance = 0.0;
    }

    voltageRms = sqrt(variance);
    currentA = voltageRms * sensorCalibration;
  }

  float getCurrentA()
  {
    return currentA;
  }

  float getVoltageRMS()
  {
    return voltageRms;
  }

  int getRawADC()
  {
    return rawAdc;
  }

  void setCalibration(float calibration)
  {
    if (calibration > 0.0f)
    {
      sensorCalibration = calibration;
    }
  }
}
