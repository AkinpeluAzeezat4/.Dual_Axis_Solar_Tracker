#pragma once

namespace pump
{
  void begin();
  void update();

  void onTankPump();
  void offTankPump();

  void onIrrigationPump();
  void offIrrigationPump();

  bool isTankPumpOn();
  bool isIrrigationPumpOn();
}
