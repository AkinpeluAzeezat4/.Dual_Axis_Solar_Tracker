#include <Arduino.h>
#include "load_relay.h"

bool relayState[6] = {false, false, false, false, false, false};

namespace load_relay
{

  void begin()
  {
  }

  void update()
  {
  }
  void setRelay(int relay, bool state)
  {
    if (relay < 0 || relay > 5)
      return;
    relayState[relay] = state;
  }

  bool getRelay1() { return relayState[0]; }
  bool getRelay2() { return relayState[1]; }
  bool getRelay3() { return relayState[2]; }
  bool getRelay4() { return relayState[3]; }
  bool getRelay5() { return relayState[4]; }
  bool getRelay6() { return relayState[5]; }
}
