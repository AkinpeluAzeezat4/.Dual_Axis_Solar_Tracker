#include <Arduino.h>
#include "ultrasonic.h"
#include "Pins.h"
#include "settings_manager/settings_manager.h"

namespace ultrasonic
{
    static float distanceCm = 0.0f;
    static uint8_t levelPercent = 0;

    static unsigned long lastRead = 0;
    static const unsigned long readInterval = 1000;

    void begin()
    {
        pinMode(Pins::ULTRASONIC_TRIG, OUTPUT);
        pinMode(Pins::ULTRASONIC_ECHO, INPUT);
        digitalWrite(Pins::ULTRASONIC_TRIG, LOW);
    }

    void update()
    {
        if (millis() - lastRead < readInterval) return;

        lastRead = millis();

        digitalWrite(Pins::ULTRASONIC_TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(Pins::ULTRASONIC_TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(Pins::ULTRASONIC_TRIG, LOW);

        unsigned long duration = pulseIn(Pins::ULTRASONIC_ECHO, HIGH, 30000);

        if (duration == 0) return;

        distanceCm = duration * 0.0343f / 2.0f;

        auto &s = settings_manager::get();

        if (s.tankEmptyCm == s.tankFullCm)
        {
            levelPercent = 0;
            return;
        }

        float percent = ((s.tankEmptyCm - distanceCm) /
                         (s.tankEmptyCm - s.tankFullCm)) *
                        100.0f;

        percent = constrain(percent, 0.0f, 100.0f);
        levelPercent = (uint8_t)percent;
    }

    float getDistanceCm()
    {
        return distanceCm;
    }

    uint8_t getLevelPercent()
    {
        return levelPercent;
    }

    bool isTankLow()
    {
        auto &s = settings_manager::get();
        return levelPercent <= s.tankLowPercent;
    }

    bool isTankFull()
    {
        auto &s = settings_manager::get();
        return levelPercent >= s.tankHighPercent;
    }
}
