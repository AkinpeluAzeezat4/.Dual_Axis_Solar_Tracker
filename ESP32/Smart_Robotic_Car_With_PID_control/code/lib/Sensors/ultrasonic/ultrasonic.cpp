#include <Arduino.h>
#include "ultrasonic.h"
#include "Pins.h"

namespace ultrasonic
{
    enum State
    {
        IDLE,
        TRIG_HIGH,
        WAIT_ECHO
    };

    static volatile unsigned long echoStartUs = 0;
    static volatile unsigned long echoPulseUs = 0;
    static volatile bool newPulse = false;

    static State state = IDLE;
    static float distanceCm = 999.0f;
    static unsigned long lastTriggerMs = 0;
    static unsigned long lastValidMs = 0;
    static unsigned long trigStartUs = 0;
    static unsigned long echoWaitStartUs = 0;

    static const unsigned long triggerIntervalMs = 70;
    static const unsigned long triggerPulseUs = 10;
    static const unsigned long echoTimeoutUs = 30000;

    static void IRAM_ATTR onEchoChange()
    {
        if (digitalRead(Pins::ULTRASONIC_ECHO) == HIGH)
        {
            echoStartUs = micros();
        }
        else
        {
            unsigned long now = micros();
            echoPulseUs = now - echoStartUs;
            newPulse = true;
        }
    }

    void begin()
    {
        pinMode(Pins::ULTRASONIC_TRIG, OUTPUT);
        pinMode(Pins::ULTRASONIC_ECHO, INPUT);
        digitalWrite(Pins::ULTRASONIC_TRIG, LOW);
        attachInterrupt(digitalPinToInterrupt(Pins::ULTRASONIC_ECHO), onEchoChange, CHANGE);
    }

    void update()
    {
        unsigned long nowMs = millis();
        unsigned long nowUs = micros();

        if (state == IDLE)
        {
            if (nowMs - lastTriggerMs >= triggerIntervalMs)
            {
                newPulse = false;
                digitalWrite(Pins::ULTRASONIC_TRIG, HIGH);
                trigStartUs = nowUs;
                state = TRIG_HIGH;
            }
            return;
        }

        if (state == TRIG_HIGH)
        {
            if (nowUs - trigStartUs >= triggerPulseUs)
            {
                digitalWrite(Pins::ULTRASONIC_TRIG, LOW);
                echoWaitStartUs = micros();
                state = WAIT_ECHO;
            }
            return;
        }

        if (state == WAIT_ECHO)
        {
            if (newPulse)
            {
                noInterrupts();
                unsigned long pulse = echoPulseUs;
                newPulse = false;
                interrupts();

                if (pulse > 100 && pulse < echoTimeoutUs)
                {
                    distanceCm = pulse * 0.0343f / 2.0f;
                    lastValidMs = millis();
                }
                else
                {
                    distanceCm = 999.0f;
                }

                lastTriggerMs = millis();
                state = IDLE;
                return;
            }

            if (micros() - echoWaitStartUs >= echoTimeoutUs)
            {
                distanceCm = 999.0f;
                lastTriggerMs = millis();
                state = IDLE;
            }
        }
    }

    float getDistanceCm()
    {
        return distanceCm;
    }

    unsigned long getLastValidMs()
    {
        return lastValidMs;
    }

    bool isObstacle(uint16_t thresholdCm)
    {
        return distanceCm > 0.0f && distanceCm <= thresholdCm;
    }

    bool hasFreshReading(uint16_t maxAgeMs)
    {
        return lastValidMs > 0 && millis() - lastValidMs <= maxAgeMs;
    }
}
