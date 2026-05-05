#include <Arduino.h>
#include "Pins.h"
#include "nepa_sense.h"

namespace nepa_sense
{
  bool simulatedAvailable = true;
  bool available = true;

  void begin()
  {
    pinMode(Pins::NEPA_SENSE, INPUT);
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
