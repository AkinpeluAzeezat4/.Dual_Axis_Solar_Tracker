#pragma once
#include <Arduino.h>

namespace temp_hum
{
  void begin();
  void update();

  float getTemperature();
  float getHumidity();
  bool isReady();
}
