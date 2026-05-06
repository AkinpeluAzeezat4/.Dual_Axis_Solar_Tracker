#include <Arduino.h>
#include "solar_level.h"
#include "Pins.h"

namespace solar_level
{
    static float voltage = 0.0f;

    static unsigned long lastReadTime = 0;
    static const unsigned long readInterval = 1000;

    static const float scaleFactor = 2.0f;

    void begin()
    {
        pinMode(Pins::SOLAR_SENSE, INPUT);
        analogReadResolution(12);
        analogSetPinAttenuation(Pins::SOLAR_SENSE, ADC_11db);
    }

    void update()
    {
        if (millis() - lastReadTime < readInterval) return;

        lastReadTime = millis();

        uint32_t sum = 0;

        for (uint8_t i = 0; i < 16; i++)
        {
            sum += analogRead(Pins::SOLAR_SENSE);
        }

        float raw = sum / 16.0f;
        float adcVoltage = (raw / 4095.0f) * 3.3f;

        voltage = adcVoltage * scaleFactor;
    }

    float getVoltage()
    {
        return voltage;
    }

    bool isPresent()
    {
        return voltage > 1.0f;
    }
}
