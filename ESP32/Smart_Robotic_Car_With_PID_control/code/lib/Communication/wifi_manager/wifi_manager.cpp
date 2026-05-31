#include <Arduino.h>
#include <WiFi.h>
#include "wifi_manager.h"

namespace wifi_manager
{
    static const char *apSsid = "softwork";
    static const char *apPassword = "softwork1234";
    static bool ready = false;

    void begin()
    {
        WiFi.mode(WIFI_AP);
        ready = WiFi.softAP(apSsid, apPassword);
    }

    void update()
    {
    }

    bool isReady()
    {
        return ready;
    }

    IPAddress getIP()
    {
        return WiFi.softAPIP();
    }

    const char *getSSID()
    {
        return apSsid;
    }
}
