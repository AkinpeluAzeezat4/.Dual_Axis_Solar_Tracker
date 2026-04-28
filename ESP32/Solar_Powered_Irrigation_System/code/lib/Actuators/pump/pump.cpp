#include <Arduino.h>
#include "pump.h"
#include "Pins.h"

namespace pump
{
  static bool tankPumpState = false;
  static bool irrigationPumpState = false;

  void begin()
  {
    pinMode(Pins::TANK_PUMP, OUTPUT);
    pinMode(Pins::IRRIGATION_PUMP, OUTPUT);

    offTankPump();
    offIrrigationPump();
  }

  void update()
  {
  }

  void onTankPump()
  {
    tankPumpState = true;
    Pins::writePin(Pins::TANK_PUMP, HIGH);
  }

  void offTankPump()
  {
    tankPumpState = false;
    Pins::writePin(Pins::TANK_PUMP, LOW);
  }

  void onIrrigationPump()
  {
    irrigationPumpState = true;
    Pins::writePin(Pins::IRRIGATION_PUMP, HIGH);
  }

  void offIrrigationPump()
  {
    irrigationPumpState = false;
    Pins::writePin(Pins::IRRIGATION_PUMP, LOW);
  }

  bool isTankPumpOn()
  {
    return tankPumpState;
  }

  bool isIrrigationPumpOn()
  {
    return irrigationPumpState;
  }
}
