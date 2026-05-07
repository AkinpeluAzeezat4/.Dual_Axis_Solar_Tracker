#pragma once

#include <Arduino.h>

namespace load_manager
{
  void begin();
  void update();

  bool isOnInverter(uint8_t relay);
  bool isLoadEnabled(uint8_t relay);

  float getLoadRatio();
  float getFuzzyRisk();

  int getCurrentLoad();
  int getEffectiveLimit();

  const char *getDecisionText();
}
