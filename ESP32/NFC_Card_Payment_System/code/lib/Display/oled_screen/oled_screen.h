#ifndef OLED_SCREEN_H
#define OLED_SCREEN_H

#include <stdint.h>

namespace oled_screen
{
  void begin(uint8_t sdaPin, uint8_t sclPin);
  static void drawTapToPayZoom();
  void update(const char *command, const char *amount = nullptr);

}

#endif
