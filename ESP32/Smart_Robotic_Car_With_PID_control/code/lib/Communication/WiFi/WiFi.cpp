#include <Arduino.h>
#include <WiFi.h>
#include "wifi_manager.h"

namespace wifi_manager
{
  static const char *ssid = "YOUR_WIFI_NAME";
  static const char *password = "YOUR_WIFI_PASSWORD";

  static unsigned long lastReconnectAttempt = 0;
  static const unsigned long reconnectInterval = 5000;

  void begin()
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  }

  void update()
  {
    if (WiFi.status() == WL_CONNECTED)
      return;

    if (millis() - lastReconnectAttempt >= reconnectInterval)
    {
      lastReconnectAttempt = millis();
      WiFi.disconnect();
      WiFi.begin(ssid, password);
    }
  }

  bool isConnected()
  {
    return WiFi.status() == WL_CONNECTED;
  }
}
