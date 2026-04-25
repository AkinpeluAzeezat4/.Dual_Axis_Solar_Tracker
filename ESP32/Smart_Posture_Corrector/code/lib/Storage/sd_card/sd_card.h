#pragma once
#include <Arduino.h>

namespace sd_card
{
  void begin();
  void update();

  bool isReady();
  void logNow();
}
