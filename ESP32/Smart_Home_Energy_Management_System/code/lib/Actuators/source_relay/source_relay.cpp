#include <Arduino.h>
#include "nepa_sense/nepa_sense.h"
#include "inverter_sense/inverter_sense.h"
#include "source_relay.h"

namespace source_relay
{
  void begin()
  {
  }

  void update()
  {
  }

  bool isNEPAAvailable()
  {
    return nepa_sense::isAvailable();
  }

  bool isInverterAvailable()
  {
    return inverter_sense::isAvailable();
  }
}
