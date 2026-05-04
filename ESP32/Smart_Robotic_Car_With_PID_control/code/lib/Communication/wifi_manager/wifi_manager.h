#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <IPAddress.h>

namespace wifi_manager
{
    void begin();
    void update();

    bool isReady();
    IPAddress getIP();
    const char *getSSID();
}

#endif
