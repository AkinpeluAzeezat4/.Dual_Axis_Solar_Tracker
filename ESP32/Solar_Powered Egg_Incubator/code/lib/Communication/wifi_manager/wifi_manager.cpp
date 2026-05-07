#include <Arduino.h>
#include <WiFi.h>
#include "wifi_manager.h"

namespace wifi_manager
{
  static const char *staSsid = "";
  static const char *staPassword = "";

  static const char *apSsid = "Egg_Incubator";
  static const char *apPassword = "12345678";

  static unsigned long lastCheck = 0;

  void begin()
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(apSsid, apPassword);

    if (strlen(staSsid) > 0)
    {
      WiFi.begin(staSsid, staPassword);
    }
  }

  void update()
  {
    if (millis() - lastCheck < 5000)
    {
      return;
    }

    lastCheck = millis();

    if (strlen(staSsid) > 0 && WiFi.status() != WL_CONNECTED)
    {
      WiFi.reconnect();
    }
  }

  String getIp()
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      return WiFi.localIP().toString();
    }

    return WiFi.softAPIP().toString();
  }

  bool isConnected()
  {
    return WiFi.status() == WL_CONNECTED;
  }
}
