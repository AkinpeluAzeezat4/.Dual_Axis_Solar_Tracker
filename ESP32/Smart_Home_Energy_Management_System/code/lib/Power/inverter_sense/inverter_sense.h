#pragma once

namespace inverter_sense
{
  void begin();
  void update();
  bool isAvailable();
  void setSimulatedAvailable(bool state);
}
