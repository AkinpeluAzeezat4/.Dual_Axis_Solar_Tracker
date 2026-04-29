#ifndef OLED_SCREEN_H
#define OLED_SCREEN_H

#include <Arduino.h>

namespace oled_screen
{
  void begin(uint8_t sdaPin, uint8_t sclPin);
  void update(const char *command, const char *amount = nullptr);
}

#endif
