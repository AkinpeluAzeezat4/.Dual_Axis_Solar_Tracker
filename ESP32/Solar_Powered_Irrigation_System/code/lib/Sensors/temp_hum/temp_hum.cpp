// #include <Arduino.h>
// #include <DHT.h>
// #include "temp_hum.h"
// #include "Pins.h"

// namespace temp_hum
// {
//   static DHT dht(Pins::DHT_DATA, DHT21);

//   static float temperature = 0.0f;
//   static float humidity = 0.0f;
//   static bool ready = false;

//   static unsigned long lastRead = 0;
//   static const unsigned long readInterval = 2000;

//   void begin()
//   {
//     dht.begin();
//   }

//   void update()
//   {
//     if (millis() - lastRead < readInterval)
//       return;
//     lastRead = millis();

//     float h = dht.readHumidity();
//     float t = dht.readTemperature();

//     if (isnan(h) || isnan(t))
//     {
//       ready = false;
//       return;
//     }

//     humidity = h;
//     temperature = t;
//     ready = true;
//   }

//   float getTemperature()
//   {
//     return temperature;
//   }

//   float getHumidity()
//   {
//     return humidity;
//   }

//   bool isReady()
//   {
//     return ready;
//   }
// }


#include <Arduino.h>
#include <DHT.h>
#include "temp_hum.h"
#include "Pins.h"

namespace temp_hum
{
    static DHT dht(Pins::DHT_DATA, DHT21);

    static float temperature = 0.0f;
    static float humidity = 0.0f;

    static bool ready = false;
    static bool error = false;

    static uint16_t failCount = 0;

    static unsigned long lastRead = 0;
    static const unsigned long readInterval = 4000;

    void begin()
    {
        pinMode(Pins::DHT_DATA, INPUT_PULLUP);
        dht.begin();
    }

    void update()
    {
        if (millis() - lastRead < readInterval) return;
        lastRead = millis();

        float h = dht.readHumidity();
        float t = dht.readTemperature();

        if (isnan(h) || isnan(t))
        {
            failCount++;
            error = true;
            return;
        }

        humidity = h;
        temperature = t;
        ready = true;
        error = false;
        failCount = 0;
    }

    float getTemperature()
    {
        return temperature;
    }

    float getHumidity()
    {
        return humidity;
    }

    bool isReady()
    {
        return ready;
    }

    bool hasError()
    {
        return error;
    }

    uint16_t getFailCount()
    {
        return failCount;
    }
}
