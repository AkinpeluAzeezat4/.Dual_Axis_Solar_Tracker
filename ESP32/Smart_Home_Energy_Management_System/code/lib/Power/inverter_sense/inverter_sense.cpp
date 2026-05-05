#include <Arduino.h>
#include "Pins.h"
#include "inverter_sense.h"

namespace inverter_sense
{
  bool simulatedAvailable = true;
  bool available = true;

  void begin()
  {
    pinMode(Pins::INVERTER_SENSE, INPUT);
    available = simulatedAvailable;
  }

  void update()
  {
    available = simulatedAvailable;
  }

  bool isAvailable()
  {
    return available;
  }

  void setSimulatedAvailable(bool state)
  {
    simulatedAvailable = state;
    available = state;
  }
}
