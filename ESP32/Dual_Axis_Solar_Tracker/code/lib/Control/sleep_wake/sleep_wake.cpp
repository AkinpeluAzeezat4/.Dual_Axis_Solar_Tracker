#include "sleep_wake.h"
#include "esp_sleep.h"
#include "pins.h"
#include "sun_sensor/sun_sensor.h"
#include "motor_driver/motor_driver.h"
#include "rotary_encoder/rotary_encoder.h"

namespace sleep_wake
{
  bool sleepRequested = false;
  bool wokeFromDeepSleep = false;

  unsigned long darkStart = 0;

  const unsigned long darkDelay = 120000;
  const uint64_t timerWakeSeconds = 600;

  void begin()
  {
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
    wokeFromDeepSleep = (cause != ESP_SLEEP_WAKEUP_UNDEFINED);
  }

  void enterSleep()
  {
    motor_driver::stopAll();

    digitalWrite(Pins::LED_LEFT, LOW);
    digitalWrite(Pins::LED_RIGHT, LOW);
    digitalWrite(Pins::LED_UP, LOW);
    digitalWrite(Pins::LED_DOWN, LOW);
    digitalWrite(Pins::LED_STATUS, LOW);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)Pins::ENC_SW, 0);
    esp_sleep_enable_timer_wakeup(timerWakeSeconds * 1000000ULL);

    delay(100);
    esp_deep_sleep_start();
  }

  void update()
  {
    sun_sensor::SensorData data = sun_sensor::getData();

    if (!data.isDark || rotary_encoder::isPressed())
    {
      darkStart = 0;
      sleepRequested = false;
      return;
    }

    if (motor_driver::getMode() != motor_driver::AUTO_TRACK)
      return;

    if (darkStart == 0)
      darkStart = millis();

    if (millis() - darkStart >= darkDelay)
      sleepRequested = true;

    if (sleepRequested)
      enterSleep();
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
