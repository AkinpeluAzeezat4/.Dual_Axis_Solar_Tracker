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

  static unsigned long lastRead = 0;
  static const unsigned long readInterval = 2000;

  void begin()
  {
    dht.begin();
  }

  void update()
  {
    if (millis() - lastRead < readInterval)
      return;
    lastRead = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
      ready = false;
      return;
    }

    humidity = h;
    temperature = t;
    ready = true;
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
}
