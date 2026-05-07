#include <Arduino.h>
#include "system_logic.h"
#include "ultrasonic/ultrasonic.h"
#include "soil_sensor/soil_sensor.h"
#include "battery_level/battery_level.h"
#include "pump/pump.h"
#include "buzzer/buzzer.h"
#include "settings_manager/settings_manager.h"

namespace system_logic
{
  static RunState state = STARTING;
  static bool tankFillActive = false;
  static bool irrigationRunning = false;
  static bool irrigationPaused = false;
  static bool tankPumpTimeoutFault = false;

  static unsigned long tankPumpStartTime = 0;
  static unsigned long irrigationStartTime = 0;
  static unsigned long irrigationPauseStartTime = 0;
  static unsigned long lastAlertTime = 0;

  static void stopIrrigation()
  {
    irrigationRunning = false;
    irrigationPaused = false;
    pump::offIrrigationPump();
  }

  static void alertIfNeeded(bool active)
  {
    if (!active)
    {
      return;
    }

    if (millis() - lastAlertTime >= 2000UL)
    {
      lastAlertTime = millis();
      buzzer::beep(150);
    }
  }

  static void updateTankPump(const settings_manager::Settings &s, unsigned long now)
  {
    uint8_t level = ultrasonic::getLevelPercent();

    if (level >= s.tankHighPercent)
    {
      tankFillActive = false;
      tankPumpTimeoutFault = false;
      pump::offTankPump();
      return;
    }

    if (level <= s.tankLowPercent && !tankPumpTimeoutFault)
    {
      if (!tankFillActive)
      {
        tankFillActive = true;
        tankPumpStartTime = now;
      }
    }

    if (tankFillActive)
    {
      unsigned long maxRun = (unsigned long)s.tankPumpMaxMinutes * 60000UL;

      if (maxRun > 0 && now - tankPumpStartTime >= maxRun)
      {
        tankFillActive = false;
        tankPumpTimeoutFault = true;
        pump::offTankPump();
        return;
      }

      pump::onTankPump();
      return;
    }

    pump::offTankPump();
  }

  static void updateIrrigation(const settings_manager::Settings &s, unsigned long now)
  {
    uint8_t level = ultrasonic::getLevelPercent();
    bool tankTooLow = level <= s.tankLowPercent;
    bool batteryTooLow = battery_level::isLow();
    bool canIrrigate = !tankTooLow && !batteryTooLow && !tankPumpTimeoutFault && soil_sensor::isReady();

    if (!canIrrigate)
    {
      stopIrrigation();
      return;
    }

    if (!soil_sensor::isDry())
    {
      stopIrrigation();
      return;
    }

    if (!irrigationRunning && !irrigationPaused)
    {
      irrigationRunning = true;
      irrigationStartTime = now;
      pump::onIrrigationPump();
    }

    if (irrigationRunning)
    {
      unsigned long runTime = (unsigned long)s.irrigationRunSeconds * 1000UL;

      if (now - irrigationStartTime >= runTime)
      {
        irrigationRunning = false;
        irrigationPaused = true;
        irrigationPauseStartTime = now;
        pump::offIrrigationPump();
      }
    }

    if (irrigationPaused)
    {
      unsigned long pauseTime = (unsigned long)s.irrigationPauseSeconds * 1000UL;

      if (now - irrigationPauseStartTime >= pauseTime)
      {
        irrigationPaused = false;
      }
    }
  }

  static void updateState(const settings_manager::Settings &s)
  {
    if (!ultrasonic::isReady())
    {
      state = SENSOR_FAULT;
      return;
    }

    if (tankPumpTimeoutFault)
    {
      state = TANK_PUMP_TIMEOUT;
      return;
    }

    if (battery_level::isLow())
    {
      state = BATTERY_LOW;
      return;
    }

    uint8_t level = ultrasonic::getLevelPercent();

    if (level <= s.tankLowPercent)
    {
      state = TANK_LOW_LOCKOUT;
      return;
    }

    if (tankFillActive)
    {
      state = FILLING_TANK;
      return;
    }

    if (irrigationRunning)
    {
      state = IRRIGATING;
      return;
    }

    if (irrigationPaused)
    {
      state = IRRIGATION_PAUSE;
      return;
    }

    state = NORMAL_IDLE;
  }

  void begin()
  {
    state = STARTING;
    tankFillActive = false;
    irrigationRunning = false;
    irrigationPaused = false;
    tankPumpTimeoutFault = false;
    pump::allOff();
  }

  void update()
  {
    auto &s = settings_manager::get();
    unsigned long now = millis();

    if (!ultrasonic::isReady())
    {
      pump::allOff();
      updateState(s);
      alertIfNeeded(ultrasonic::hasError());
      return;
    }

    if (battery_level::isLow())
    {
      pump::allOff();
      tankFillActive = false;
      stopIrrigation();
      updateState(s);
      alertIfNeeded(true);
      return;
    }

    updateTankPump(s, now);
    updateIrrigation(s, now);
    updateState(s);

    bool alarm = state == TANK_LOW_LOCKOUT || state == SENSOR_FAULT || state == TANK_PUMP_TIMEOUT || state == BATTERY_LOW;
    alertIfNeeded(alarm);
  }

  RunState getState()
  {
    return state;
  }

  const char *getStatusText()
  {
    switch (state)
    {
    case STARTING:
      return "Starting";
    case NORMAL_IDLE:
      return "Auto Idle";
    case FILLING_TANK:
      return "Filling Tank";
    case TANK_LOW_LOCKOUT:
      return "Tank Low Stop";
    case IRRIGATING:
      return "Irrigating";
    case IRRIGATION_PAUSE:
      return "Irr Pause";
    case SENSOR_FAULT:
      return "Sensor Fault";
    case TANK_PUMP_TIMEOUT:
      return "Pump Timeout";
    case BATTERY_LOW:
      return "Battery Low";
    default:
      return "Unknown";
    }
  }

  bool isTankFillActive()
  {
    return tankFillActive;
  }

  bool isIrrigationActive()
  {
    return irrigationRunning;
  }

  bool hasFault()
  {
    return state == SENSOR_FAULT || state == TANK_PUMP_TIMEOUT;
  }
}
