#include <Arduino.h>
#include "Pins.h"
#include "load_relay.h"
#include "current_sensor/current_sensor.h"
#include "temp_sensor/temp_sensor.h"
#include "vibration_sensor/vibration_sensor.h"
#include "buzzer/buzzer.h"
#include "error_handling/error_handling.h"

namespace load_relay
{
  static const bool RELAY_ACTIVE_HIGH = true;

  static const float MAX_CURRENT_A = 5.0;
  static const float MAX_TEMPERATURE_C = 70.0;
  static const float MAX_VIBRATION_RMS_G = 2.5;

  static bool relayOn = true;
  static bool userRelayState = true;
  static bool fault = false;

  static void writeRelay(bool state)
  {
    relayOn = state;

    bool level = RELAY_ACTIVE_HIGH ? state : !state;
    Pins::writePin(Pins::RELAY, level);
  }

  void begin()
  {
    pinMode(Pins::RELAY, OUTPUT);
    writeRelay(userRelayState);
  }

  void update()
  {
    bool currentFault = current_sensor::getCurrentA() > MAX_CURRENT_A;
    bool tempFault = temp_sensor::isValid() && temp_sensor::getTemperatureC() > MAX_TEMPERATURE_C;
    bool vibrationFault = vibration_sensor::isReady() && vibration_sensor::getVibrationRMS() > MAX_VIBRATION_RMS_G;

    fault = currentFault || tempFault || vibrationFault;

    error_handling::setCodeError(fault);

    if (fault)
    {
      writeRelay(false);
      buzzer::startAlarm();
      return;
    }

    buzzer::stopAlarm();
    writeRelay(userRelayState);
  }

  void setOn(bool state)
  {
    userRelayState = state;

    if (!fault)
    {
      writeRelay(userRelayState);
    }
  }

  void turnOn()
  {
    setOn(true);
  }

  void turnOff()
  {
    setOn(false);
  }

  bool isOn()
  {
    return relayOn;
  }

  bool isFault()
  {
    return fault;
  }

  void clearFault()
  {
    fault = false;

    if (!fault)
    {
      writeRelay(userRelayState);
      buzzer::stopAlarm();
    }
  }
}
