#include "battery_level.h"
#include "pins.h"

const uint8_t BATTERY_PIN = Pins::GPIO34; // battery voltage input pin

namespace battery_level
{
    static float v = 0.0;
    static uint8_t pct = 0;
    static bool low = false;

    void begin()
    {
        pinMode(BATTERY_PIN, INPUT);
    }

    void update()
    {
        int raw = analogRead(BATTERY_PIN);

        v = (raw / 4095.0) * 3.3 * 2.0;
        pct = constrain(map(v * 100, 330, 420, 0, 100), 0, 100);

        low = (pct <= 20);
    }

    float voltage() { return v; }
    uint8_t percentage() { return pct; }
    bool isLow() { return low; }
}
