#include <Arduino.h>
#include "led_indicator.h"
#include "Pins.h"

namespace led_indicator
{
    static uint8_t batteryPercentage = 0;

    static const int powerPwmChannel = 0;
    static const int powerPwmFreq = 5000;
    static const int powerPwmResolution = 8;

    static int powerBrightness = 0;
    static bool powerRising = true;
    static bool powerBreathingEnabled = true;

    static unsigned long lastPowerUpdate = 0;
    static const unsigned long powerStepTime = 5;

    static void updateBatteryLeds()
    {
        digitalWrite(Pins::BATTERY_LED1, batteryPercentage >= 25 ? HIGH : LOW);
        digitalWrite(Pins::BATTERY_LED2, batteryPercentage >= 50 ? HIGH : LOW);
        digitalWrite(Pins::BATTERY_LED3, batteryPercentage >= 75 ? HIGH : LOW);
        digitalWrite(Pins::BATTERY_LED4, batteryPercentage >= 95 ? HIGH : LOW);
    }

    static void updatePowerBreathing()
    {
        if (!powerBreathingEnabled)
        {
            ledcWrite(powerPwmChannel, 0);
            return;
        }

        unsigned long now = millis();
        if (now - lastPowerUpdate < powerStepTime) return;
        lastPowerUpdate = now;

        if (powerRising)
        {
            powerBrightness++;
            if (powerBrightness >= 255)
            {
                powerBrightness = 255;
                powerRising = false;
            }
        }
        else
        {
            powerBrightness--;
            if (powerBrightness <= 0)
            {
                powerBrightness = 0;
                powerRising = true;
            }
        }

        ledcWrite(powerPwmChannel, powerBrightness);
    }

    void begin()
    {
        pinMode(Pins::BATTERY_LED1, OUTPUT);
        pinMode(Pins::BATTERY_LED2, OUTPUT);
        pinMode(Pins::BATTERY_LED3, OUTPUT);
        pinMode(Pins::BATTERY_LED4, OUTPUT);

        ledcSetup(powerPwmChannel, powerPwmFreq, powerPwmResolution);
        ledcAttachPin(Pins::POWER_LED, powerPwmChannel);

        setBatteryLevel(0);
        ledcWrite(powerPwmChannel, 0);
    }

    void update()
    {
        updateBatteryLeds();
        updatePowerBreathing();
    }

    void setBatteryLevel(uint8_t percentage)
    {
        batteryPercentage = constrain(percentage, 0, 100);
    }

    void enablePowerBreathing(bool state)
    {
        powerBreathingEnabled = state;
    }
}
