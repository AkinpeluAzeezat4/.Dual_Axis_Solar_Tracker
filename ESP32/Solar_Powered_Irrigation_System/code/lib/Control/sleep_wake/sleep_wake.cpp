#include <Arduino.h>
#include <esp_sleep.h>
#include "sleep_wake.h"
#include "Pins.h"
#include "pump/pump.h"

namespace sleep_wake
{
  static bool sleepRequested = false;
  static bool wokeFromDeepSleep = false;

  void begin()
  {
    pinMode(Pins::ENCODER_SW, INPUT_PULLUP);

    esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);

    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    wokeFromDeepSleep = (cause != ESP_SLEEP_WAKEUP_UNDEFINED);
  }

  void update()
  {
    if (!sleepRequested)
      return;

    sleepRequested = false;

    pump::offTankPump();
    pump::offIrrigationPump();

    delay(100);
    esp_deep_sleep_start();
  }

  void requestSleep(bool state)
  {
    sleepRequested = state;
  }

  bool wokeFromSleep()
  {
    return wokeFromDeepSleep;
  }
}
