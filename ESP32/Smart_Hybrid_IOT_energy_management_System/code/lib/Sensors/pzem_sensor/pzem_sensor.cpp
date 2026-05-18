#include <Arduino.h>
#include <PZEM004Tv30.h>
#include "Pins.h"
#include "pzem_sensor.h"

namespace pzem_sensor
{
  PZEM004Tv30 pzem(Serial2, Pins::PZEM_RX, Pins::PZEM_TX);

  float voltage = 0.0f;
  float current = 0.0f;
  float power = 0.0f;
  float energy = 0.0f;
  float frequency = 0.0f;
  float powerFactor = 0.0f;

  bool ready = false;
  bool validData = false;

  unsigned long lastUpdate = 0;
  const unsigned long updateInterval = 1000;

  bool validNumber(float value)
  {
    return !isnan(value) && isfinite(value) && value >= 0.0f;
  }

  void begin()
  {
    Serial2.begin(9600, SERIAL_8N1, Pins::PZEM_RX, Pins::PZEM_TX);
    ready = true;
    validData = false;
    lastUpdate = 0;
  }

  void update()
  {
    unsigned long now = millis();

    if (now - lastUpdate < updateInterval)
      return;

    lastUpdate = now;

    float v = pzem.voltage();
    float c = pzem.current();
    float p = pzem.power();
    float e = pzem.energy();
    float f = pzem.frequency();
    float pf = pzem.pf();

    bool ok = validNumber(v) && validNumber(c) && validNumber(p);

    if (!ok)
    {
      validData = false;
      voltage = 0.0f;
      current = 0.0f;
      power = 0.0f;
      return;
    }

    voltage = v;
    current = c;
    power = p;

    if (validNumber(e))
      energy = e;

    if (validNumber(f))
      frequency = f;

    if (!isnan(pf) && isfinite(pf))
      powerFactor = pf;

    validData = true;
  }

  float getVoltage()
  {
    return voltage;
  }

  float getCurrent()
  {
    return current;
  }

  float getPower()
  {
    return power;
  }

  float getEnergy()
  {
    return energy;
  }

  float getFrequency()
  {
    return frequency;
  }

  float getPowerFactor()
  {
    return powerFactor;
  }

  int getTotalPower()
  {
    return (int)(power + 0.5f);
  }

  bool isReady()
  {
    return ready;
  }

  bool hasValidData()
  {
    return validData;
  }

  void resetEnergy()
  {
    pzem.resetEnergy();
    energy = 0.0f;
  }
}
