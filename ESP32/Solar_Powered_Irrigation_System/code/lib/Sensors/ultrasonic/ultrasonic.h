#pragma once
#include <Arduino.h>

namespace ultrasonic
{
  void begin();
  void update();

  float getDistanceCm();
  uint8_t getLevelPercent();

  bool isReady();
  bool hasError();
  bool isTankLow();
  bool isTankFull();
}
