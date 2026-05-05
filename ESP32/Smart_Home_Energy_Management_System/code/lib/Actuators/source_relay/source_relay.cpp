#include <Arduino.h>
#include "source_relay.h"

namespace source_relay
{
  bool phcnAvailable = true;
  bool inverterAvailable = true;

  void begin()
  {
  }

  void update()
  {
  }

  bool isPHCNAvailable()
  {
    return phcnAvailable;
  }

  bool isInverterAvailable()
  {
    return inverterAvailable;
  }

  void setPHCNAvailable(bool available)
  {
    phcnAvailable = available;
  }

  void setInverterAvailable(bool available)
  {
    inverterAvailable = available;
  }
}
