#ifndef LED_INDICATOR_H
#define LED_INDICATOR_H

#include <stdint.h>

namespace led_indicator
{
  void begin(uint8_t pin);
  void update(const char *);
}

#endif
