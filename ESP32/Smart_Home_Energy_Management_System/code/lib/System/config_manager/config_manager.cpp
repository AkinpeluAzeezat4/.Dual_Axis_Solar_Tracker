#include <Arduino.h>
#include <Preferences.h>
#include "config_manager.h"

namespace config_manager
{
  Preferences prefs;
  int relayPower[6] = {500, 700, 900, 1200, 1500, 2000};
  int inverterPower = 5000;
  int systemPower = 6000;
  int loadMarginPercent = 90;
  bool configured = false;

  int clampValue(int value, int low, int high)
  {
    if (value < low) return low;
    if (value > high) return high;
    return value;
  }

  void begin()
  {
    prefs.begin("shems", false);
    configured = prefs.getBool("configured", false);

    for (int i = 0; i < 6; i++)
    {
      char key[8];
      snprintf(key, sizeof(key), "r%d", i + 1);
      relayPower[i] = prefs.getInt(key, relayPower[i]);
      relayPower[i] = clampValue(relayPower[i], 0, 4000);
    }

    inverterPower = clampValue(prefs.getInt("inv", inverterPower), 500, 20000);
    systemPower = clampValue(prefs.getInt("sys", systemPower), 0, 30000);
    loadMarginPercent = clampValue(prefs.getInt("margin", loadMarginPercent), 50, 100);
  }

  void update()
  {
  }

  void save()
  {
    for (int i = 0; i < 6; i++)
    {
      char key[8];
      snprintf(key, sizeof(key), "r%d", i + 1);
      prefs.putInt(key, relayPower[i]);
    }

    prefs.putInt("inv", inverterPower);
    prefs.putInt("sys", systemPower);
    prefs.putInt("margin", loadMarginPercent);
    prefs.putBool("configured", true);
    configured = true;
  }

  void resetDefaults()
  {
    relayPower[0] = 500;
    relayPower[1] = 700;
    relayPower[2] = 900;
    relayPower[3] = 1200;
    relayPower[4] = 1500;
    relayPower[5] = 2000;
    inverterPower = 5000;
    systemPower = 6000;
    loadMarginPercent = 90;
    save();
  }

  int getRelayPower(uint8_t index)
  {
    if (index >= 6) return 0;
    return relayPower[index];
  }

  void setRelayPower(uint8_t index, int value)
  {
    if (index >= 6) return;
    relayPower[index] = clampValue(value, 0, 4000);
  }

  int getInverterPower()
  {
    return inverterPower;
  }

  void setInverterPower(int value)
  {
    inverterPower = clampValue(value, 500, 20000);
  }

  int getSystemPower()
  {
    return systemPower;
  }

  void setSystemPower(int value)
  {
    systemPower = clampValue(value, 0, 30000);
  }

  int getLoadMarginPercent()
  {
    return loadMarginPercent;
  }

  void setLoadMarginPercent(int value)
  {
    loadMarginPercent = clampValue(value, 50, 100);
  }

  bool isConfigured()
  {
    return configured;
  }
}
