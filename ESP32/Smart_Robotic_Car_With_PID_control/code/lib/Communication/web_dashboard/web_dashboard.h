#ifndef WEB_DASHBOARD_H
#define WEB_DASHBOARD_H

#include <Arduino.h>

namespace web_dashboard
{
    void begin();
    void update();
    String getIp();
}

#endif
