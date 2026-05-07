#include <Arduino.h>
#include "Pins.h"
#include "gpio_expander/gpio_expander.h"
#include "load_relay.h"

namespace load_relay
{
  const uint8_t sourcePins[6] = {
      Pins::EXP_RELAY_1_SOURCE,
      Pins::EXP_RELAY_2_SOURCE,
      Pins::EXP_RELAY_3_SOURCE,
      Pins::EXP_RELAY_4_SOURCE,
      Pins::EXP_RELAY_5_SOURCE,
      Pins::EXP_RELAY_6_SOURCE};

  const uint8_t loadPins[6] = {
      Pins::EXP_RELAY_1_LOAD,
      Pins::EXP_RELAY_2_LOAD,
      Pins::EXP_RELAY_3_LOAD,
      Pins::EXP_RELAY_4_LOAD,
      Pins::EXP_RELAY_5_LOAD,
      Pins::EXP_RELAY_6_LOAD};

  bool onInverterState[6] = {true, true, true, true, true, true};
  bool loadEnabledState[6] = {true, true, true, true, true, true};

  bool dirty = true;

  void applyRelay(int index)
  {
    if (!gpio_expander::isReady())
      return;

    bool sourceRelayEnergized = !onInverterState[index];
    bool loadRelayEnergized = loadEnabledState[index];

    gpio_expander::digitalWrite(sourcePins[index], sourceRelayEnergized);
    gpio_expander::digitalWrite(loadPins[index], loadRelayEnergized);
  }

  void begin()
  {
    if (!gpio_expander::isReady())
      return;

    for (int i = 0; i < 6; i++)
    {
      gpio_expander::pinMode(sourcePins[i], OUTPUT);
      gpio_expander::pinMode(loadPins[i], OUTPUT);
    }

    dirty = true;
    update();
  }

  void update()
  {
    if (!dirty)
      return;

    if (!gpio_expander::isReady())
      return;

    for (int i = 0; i < 6; i++)
      applyRelay(i);

    dirty = false;
  }

  void setRelay(int relay, bool onInverter)
  {
    if (relay < 0 || relay > 5)
      return;

    if (onInverterState[relay] != onInverter)
    {
      onInverterState[relay] = onInverter;
      dirty = true;
    }
  }

  void setLoadEnabled(int relay, bool enabled)
  {
    if (relay < 0 || relay > 5)
      return;

    if (loadEnabledState[relay] != enabled)
    {
      loadEnabledState[relay] = enabled;
      dirty = true;
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

  void enableAllLoads()
  {
    for (int i = 0; i < 6; i++)
      setLoadEnabled(i, true);
  }

  void disableAllLoads()
  {
    for (int i = 0; i < 6; i++)
      setLoadEnabled(i, false);
  }

  bool isOnInverter(int relay)
  {
    if (relay < 0 || relay > 5)
      return false;

    return onInverterState[relay];
  }

  bool isLoadEnabled(int relay)
  {
    if (relay < 0 || relay > 5)
      return false;

    return loadEnabledState[relay];
  }

  bool getRelay1() { return onInverterState[0]; }
  bool getRelay2() { return onInverterState[1]; }
  bool getRelay3() { return onInverterState[2]; }
  bool getRelay4() { return onInverterState[3]; }
  bool getRelay5() { return onInverterState[4]; }
  bool getRelay6() { return onInverterState[5]; }
}
