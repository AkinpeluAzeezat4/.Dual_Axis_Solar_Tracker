#include <Arduino.h>
#include "reset.h"
#include "rotary_encoder/rotary_encoder.h"
#include "sun_sensor/sun_sensor.h"

namespace reset
{
  unsigned long pressStart = 0;
  bool active = false;

  void begin()
  {
  }

  void update()
  {
    if (rotary_encoder::isPressed())
    {
      if (!active)
      {
        active = true;
        pressStart = millis();
      }

      if (millis() - pressStart > 7000)
      {
        sun_sensor::setNightThreshold(950);
        sun_sensor::setDeadbandOffset(0);
        ESP.restart();
      }
    }
    else
    {
      active = false;
    }
  }
}
