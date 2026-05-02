#include <Arduino.h>
#include "battery_level.h"
#include "Pins.h"

namespace battery_level
{
  static uint8_t batteryPin = Pins::UNUSED_PIN;
  static float scaleFactor = 2.0f;
  static float voltage = 0.0f;
  static uint8_t percent = 0;
  static bool ready = false;
  static unsigned long lastRead = 0;

  static const unsigned long readInterval = 1000;

  static uint8_t voltageToPercent(float value)
  {
    if (value <= 3.2f)
      return 0;

    if (value >= 4.2f)
      return 100;

    return (uint8_t)(((value - 3.2f) / 1.0f) * 100.0f);
  }

  static void readBattery()
  {
    int raw = analogRead(batteryPin);

    float adcVoltage = (raw / 4095.0f) * 3.3f;
    voltage = adcVoltage * scaleFactor;
    percent = voltageToPercent(voltage);
  }

  void begin(uint8_t pin, float scale)
  {
    batteryPin = pin;
    scaleFactor = scale;
    ready = Pins::valid(batteryPin);

    if (!ready)
      return;

    pinMode(batteryPin, INPUT);
    analogReadResolution(12);
    analogSetPinAttenuation(batteryPin, ADC_11db);

    readBattery();
    lastRead = millis();
  }

  void update()
  {
    if (!ready)
      return;

    if (millis() - lastRead < readInterval)
      return;

    lastRead = millis();
    readBattery();
  }

  float getVoltage()
  {
    return voltage;
  }

  uint8_t getPercent()
  {
    return percent;
  }

  bool isReady()
  {
    return ready;
  }
}
