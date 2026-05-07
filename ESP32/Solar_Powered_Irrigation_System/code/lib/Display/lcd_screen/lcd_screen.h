#pragma once
#include <Arduino.h>

namespace lcd_screen
{
  void begin();
  void update();
  void showMessage(const char *line1, const char *line2, const char *line3, const char *line4);
  void setAutoRefresh(bool state);
  bool isAutoRefreshEnabled();
}
