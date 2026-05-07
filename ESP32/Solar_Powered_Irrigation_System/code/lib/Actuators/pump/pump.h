#pragma once
#include <Arduino.h>

namespace pump
{
  void begin();
  void update();

  void onTankPump();
  void offTankPump();

  void onIrrigationPump();
  void offIrrigationPump();

  void allOff();

  bool isTankPumpOn();
  bool isIrrigationPumpOn();
}
