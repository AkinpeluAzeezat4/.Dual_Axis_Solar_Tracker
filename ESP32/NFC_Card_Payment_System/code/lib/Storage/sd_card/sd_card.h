#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>

namespace sd_card
{
  void begin(uint8_t csPin, uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin);
  void update();

  bool isReady();
  bool exists(const char *path);

  bool writeText(const char *path, const String &text);
  bool appendLine(const char *path, const String &line);
  String readText(const char *path);
  bool removeFile(const char *path);
}

#endif
