#include <Arduino.h>
#include "pzem_sensor.h"
#include "config_manager/config_manager.h"

namespace pzem_sensor
{
  float voltage = 220.0f;
  float current = 0.0f;
  float power = 6000.0f;
  float energy = 0.0f;
  unsigned long lastEnergyUpdate = 0;

  void begin()
  {
    lastEnergyUpdate = millis();
    power = config_manager::getSystemPower();
  }

  void update()
  {
    unsigned long now = millis();
    unsigned long elapsed = now - lastEnergyUpdate;

    if (elapsed >= 1000)
    {
      power = config_manager::getSystemPower();
      current = voltage > 1.0f ? power / voltage : 0.0f;
      energy += (power * (elapsed / 3600000.0f));
      lastEnergyUpdate = now;
    }
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

  int getTotalPower()
  {
    return (int)power;
  }

  void setSimulatedPower(int watts)
  {
    if (watts < 0) watts = 0;
    power = watts;
    config_manager::setSystemPower(watts);
  }
}
