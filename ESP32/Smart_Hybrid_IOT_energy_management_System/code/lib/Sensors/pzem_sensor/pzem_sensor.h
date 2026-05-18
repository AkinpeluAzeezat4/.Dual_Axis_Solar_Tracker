#pragma once

namespace pzem_sensor
{
  void begin();
  void update();

  float getVoltage();
  float getCurrent();
  float getPower();
  float getEnergy();
  float getFrequency();
  float getPowerFactor();

  int getTotalPower();

  bool isReady();
  bool hasValidData();

  void resetEnergy();
}
