#include <Arduino.h>
#include "battery_level.h"
#include "Pins.h"
#include "settings_manager/settings_manager.h"

namespace battery_level
{
    static float voltage = 0.0f;
    static uint8_t percentage = 0;

    static unsigned long lastReadTime = 0;
    static const unsigned long readInterval = 1000;

    static const float scaleFactor = 2.0f;

    void begin()
    {
        pinMode(Pins::BATTERY_SENSE, INPUT);
        analogReadResolution(12);
        analogSetPinAttenuation(Pins::BATTERY_SENSE, ADC_11db);
    }

    void update()
    {
        if (millis() - lastReadTime < readInterval) return;

        lastReadTime = millis();

        uint32_t sum = 0;

        for (uint8_t i = 0; i < 16; i++)
        {
            sum += analogRead(Pins::BATTERY_SENSE);
        }

        float raw = sum / 16.0f;
        float adcVoltage = (raw / 4095.0f) * 3.3f;

        voltage = adcVoltage * scaleFactor;

        auto &s = settings_manager::get();

        if (s.batteryFullV <= s.batteryEmptyV)
        {
            percentage = 0;
            return;
        }

        float pct = ((voltage - s.batteryEmptyV) / (s.batteryFullV - s.batteryEmptyV)) * 100.0f;
        pct = constrain(pct, 0.0f, 100.0f);

        percentage = (uint8_t)pct;
    }

    float getVoltage()
    {
        return voltage;
    }

    uint8_t getPercentage()
    {
        return percentage;
    }

    bool isLow()
    {
        return percentage <= 15;
    }
}
