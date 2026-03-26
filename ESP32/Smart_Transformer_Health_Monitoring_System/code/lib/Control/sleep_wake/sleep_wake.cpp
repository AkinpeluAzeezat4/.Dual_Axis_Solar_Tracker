#include "sleep_wake/sleep_wake.h"
#include "esp_sleep.h"

namespace sleep_wake
{

    bool sleepRequested = false;
    bool wokeFromDeepSleep = false;

    void begin()
    {
        esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
        wokeFromDeepSleep = (cause != ESP_SLEEP_WAKEUP_UNDEFINED);
    }

    void update()
    {
        if (sleepRequested)
        {
            sleepRequested = false;
            esp_deep_sleep_start();
        }
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
