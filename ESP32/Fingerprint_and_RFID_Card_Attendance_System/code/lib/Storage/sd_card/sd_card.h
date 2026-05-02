#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>

namespace sd_card
{
  void begin();
  void update();

  bool isReady();

  bool appendLine(const String &path, const String &line);
  bool writeFile(const String &path, const String &content);
  bool ensureFile(const String &path, const String &header);

  String readFile(const String &path, size_t maxBytes);
  String getConfigValue(const String &key, const String &fallback);

  uint32_t countDataLines(const String &path);

  bool clearAttendance();

  uint16_t getUserFingerId(const String &userId);
  bool deleteUser(const String &userId);

  uint64_t totalBytes();
  uint64_t usedBytes();
}

#endif
