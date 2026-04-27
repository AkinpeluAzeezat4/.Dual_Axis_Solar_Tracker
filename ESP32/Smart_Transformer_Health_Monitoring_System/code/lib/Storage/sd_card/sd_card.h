#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>

namespace sd_card
{
  void begin();
  void update();

  void logNow();

  bool isReady();

  void setLogInterval(unsigned long intervalMs);
  const char *getFileName();
}

#endif
