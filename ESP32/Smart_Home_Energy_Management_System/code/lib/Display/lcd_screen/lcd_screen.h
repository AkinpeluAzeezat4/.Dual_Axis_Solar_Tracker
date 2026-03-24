#pragma once

namespace lcd_screen
{
  void begin();
  void update(const char *);
  void drawRoundedFrame(int x, int y, int w, int h, int r);
  void drawSwitchStatus(int x, int y, int relayNum, bool state);
  void drawSwitchStatus(int x, int y, int relayNum, bool state);
  void drawLoadStatusScreen();
  void drawConsumptionScreen();
  void drawSettingsScreen();
  int getSelectedItem();
  void updateMenu();
  void drawRelaySetupScreen();
  void relayPowerSetup();
  bool isRelaySetupFinished();
  void resetRelaySetup();
  int getRelayPower(int relay);
  void drawSystemSettingsScreen();
  void systemSettings();
  int getSystemPower();
  int getInverterPower();
}
