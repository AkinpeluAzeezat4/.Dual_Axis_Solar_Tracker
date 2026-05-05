#pragma once

namespace source_relay
{
  void begin();
  void update();

  bool isPHCNAvailable();
  bool isInverterAvailable();

  void setPHCNAvailable(bool available);
  void setInverterAvailable(bool available);
}
