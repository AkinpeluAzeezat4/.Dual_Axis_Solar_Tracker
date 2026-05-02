#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include <Arduino.h>

namespace led_indicator
{
  enum Mode
  {
    OFF,
    ON,
    BREATHING,
    BLINK_FAST,
    BLINK_SLOW
  };

  void begin(uint8_t ledPin);
  void update();

  void setMode(Mode mode);
  Mode getMode();

  void off();
  void on();
}

#endif
