#include <Arduino.h>
#include "sleep_wake.h"
#include "Pins.h"
#include "button.h"
#include "posture_logic.h"
#include "vibration_motor.h"
#include "buzzer.h"
#include "led_indicator.h"

namespace sleep_wake
{
  static bool sleeping = false;
  static unsigned long lastActivity = 0;

  static const unsigned long sleepDelay = 300000;

  void begin()
  {
    sleeping = false;
    lastActivity = millis();

    esp_sleep_enable_ext0_wakeup((gpio_num_t)Pins::USER_BUTTON, 0);
  }

  void update()
  {
    if (button::isPressed())
    {
      resetActivity();
    }

    if (posture_logic::getState() == posture_logic::BAD_POSTURE ||
        posture_logic::getState() == posture_logic::ALERTING)
    {
      resetActivity();
    }

    if (millis() - lastActivity >= sleepDelay)
    {
      sleepNow();
    }
  }

  void resetActivity()
  {
    lastActivity = millis();
  }

  void sleepNow()
  {
    sleeping = true;

    vibration_motor::setAlert(false);
    buzzer::setAlert(false);
    led_indicator::setBreathing(false);
    led_indicator::setFault(false);

    delay(100);
    esp_deep_sleep_start();
  }

  bool isSleeping()
  {
    return sleeping;
  }
}
