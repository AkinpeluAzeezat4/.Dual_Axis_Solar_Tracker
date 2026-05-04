#include <Arduino.h>
#include <esp_sleep.h>
#include "sleep_wake/sleep_wake.h"

namespace sleep_wake
{
    static bool sleepRequested = false;
    static bool wokeFromDeepSleep = false;

    void begin()
    {
        esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
        wokeFromDeepSleep = cause != ESP_SLEEP_WAKEUP_UNDEFINED;
    }

    void update()
    {
        if (!sleepRequested) return;
        sleepRequested = false;
        esp_deep_sleep_start();
    }

    void requestSleep(bool state)
    {
        sleepRequested = state;
    }

    bool wokeFromSleep()
    {
        return wokeFromDeepSleep;
    }
}
