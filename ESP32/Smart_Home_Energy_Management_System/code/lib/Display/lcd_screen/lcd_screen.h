#pragma once

namespace lcd_screen
{
  void begin();
  void update(const char *command);

  int getSelectedItem();

  bool isRelaySetupFinished();
  void resetRelaySetup();

  int getRelayPower(int relay);
  int getSystemPower();
  int getInverterPower();
}
