#ifndef WIFI_SERVICE_H
#define WIFI_SERVICE_H

#include <Arduino.h>

namespace wifi_service
{
  void begin();
  void update();

  bool isConnected();
  bool isReady();
  String getIP();
}

#endif
