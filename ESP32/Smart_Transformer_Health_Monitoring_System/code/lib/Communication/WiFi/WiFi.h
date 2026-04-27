#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

namespace wifi_manager
{
  void begin();
  void update();

  bool isConnected();
}

#endif
