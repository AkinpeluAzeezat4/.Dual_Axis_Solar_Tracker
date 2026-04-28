#include <Arduino.h>
#include "system_logic.h"
#include "ultrasonic/ultrasonic.h"
#include "soil_sensor/soil_sensor.h"
#include "pump/pump.h"
#include "settings_manager/settings_manager.h"

namespace system_logic
{
  static unsigned long irrigationStartTime = 0;
  static unsigned long irrigationPauseStart = 0;
  static bool irrigationRunning = false;
  static bool irrigationPaused = false;

  void begin()
  {
    irrigationRunning = false;
    irrigationPaused = false;
    pump::offTankPump();
    pump::offIrrigationPump();
  }

  void update()
  {
    auto &s = settings_manager::get();
    unsigned long now = millis();

    if (ultrasonic::isTankLow())
    {
      pump::onTankPump();
    }

    if (ultrasonic::isTankFull())
    {
      pump::offTankPump();
    }

    if (soil_sensor::isDry() && !irrigationRunning && !irrigationPaused)
    {
      irrigationRunning = true;
      irrigationStartTime = now;
      pump::onIrrigationPump();
    }

    if (irrigationRunning)
    {
      if (now - irrigationStartTime >= (unsigned long)s.irrigationRunSeconds * 1000UL)
      {
        irrigationRunning = false;
        irrigationPaused = true;
        irrigationPauseStart = now;
        pump::offIrrigationPump();
      }
    }

    if (irrigationPaused)
    {
      if (now - irrigationPauseStart >= (unsigned long)s.irrigationPauseSeconds * 1000UL)
      {
        irrigationPaused = false;
      }
    }

    if (!soil_sensor::isDry())
    {
      irrigationRunning = false;
      irrigationPaused = false;
      pump::offIrrigationPump();
    }
  }
}
