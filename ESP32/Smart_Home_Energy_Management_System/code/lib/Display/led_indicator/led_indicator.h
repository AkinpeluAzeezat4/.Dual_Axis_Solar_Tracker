#pragma once

namespace led_indicator
{
  void begin(int pin);
  void update();

  void setEnabled(bool enabled);
  bool isEnabled();
}
