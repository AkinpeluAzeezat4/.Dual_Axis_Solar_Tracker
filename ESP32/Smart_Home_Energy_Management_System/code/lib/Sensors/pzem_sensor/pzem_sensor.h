#pragma once

namespace pzem_sensor
{
  void begin();
  void update();
  float getVoltage();
  float getCurrent();
  float getPower();
  float getEnergy();
  int getTotalPower();
  void setSimulatedPower(int watts);
}
