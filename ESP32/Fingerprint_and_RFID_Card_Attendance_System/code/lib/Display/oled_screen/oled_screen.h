#ifndef OLED_SCREEN_H
#define OLED_SCREEN_H

#include <Arduino.h>

namespace oled_screen
{
  void begin();
  void update();

  void show(const String &title, const String &line1, const String &line2, const String &line3);

  void showBoot();
  void showReady(const String &ip);
  void showError(const String &message);

  bool isReady();
}

#endif
