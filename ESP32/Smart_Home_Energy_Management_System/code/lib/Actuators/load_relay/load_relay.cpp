#include <Arduino.h>
#include "Pins.h"
#include "load_relay.h"

namespace load_relay
{
  const uint8_t relayPins[6] = {
      Pins::RELAY_1,
      Pins::RELAY_2,
      Pins::RELAY_3,
      Pins::RELAY_4,
      Pins::RELAY_5,
      Pins::RELAY_6};

  bool relayState[6] = {true, true, true, true, true, true};
  bool relayDirty = true;

  const bool relayActiveHigh = true;

  void writeRelay(int index)
  {
    if (relayActiveHigh)
      digitalWrite(relayPins[index], relayState[index] ? HIGH : LOW);
    else
      digitalWrite(relayPins[index], relayState[index] ? LOW : HIGH);
  }

  void begin()
  {
    for (int i = 0; i < 6; i++)
    {
      pinMode(relayPins[i], OUTPUT);
      writeRelay(i);
    }

    relayDirty = false;
  }

  void update()
  {
    if (!relayDirty)
      return;

    for (int i = 0; i < 6; i++)
      writeRelay(i);

    relayDirty = false;
  }

  void setRelay(int relay, bool onInverter)
  {
    if (relay < 0 || relay > 5)
      return;

    if (relayState[relay] != onInverter)
    {
      relayState[relay] = onInverter;
      relayDirty = true;
    }
  }

  void setAllInverter()
  {
    for (int i = 0; i < 6; i++)
      setRelay(i, true);
  }

  void setAllPHCN()
  {
    for (int i = 0; i < 6; i++)
      setRelay(i, false);
  }

  bool isOnInverter(int relay)
  {
    if (relay < 0 || relay > 5)
      return false;

    return relayState[relay];
  }

  bool getRelay1()
  {
    return relayState[0];
  }

  bool getRelay2()
  {
    return relayState[1];
  }

  bool getRelay3()
  {
    return relayState[2];
  }

  bool getRelay4()
  {
    return relayState[3];
  }

  bool getRelay5()
  {
    return relayState[4];
  }

  bool getRelay6()
  {
    return relayState[5];
  }
}
