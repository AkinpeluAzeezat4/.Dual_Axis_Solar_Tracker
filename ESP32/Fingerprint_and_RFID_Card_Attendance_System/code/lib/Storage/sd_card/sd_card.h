#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <FS.h>

namespace sd_card
{
  void begin();
  void update();

  bool isReady();
  bool isSdCardReady();
  bool isInternalReady();
  String getStorageName();

  bool appendLine(const String &path, const String &line);
  bool writeFile(const String &path, const String &content);
  bool ensureFile(const String &path, const String &header);
  bool exists(const String &path);
  File openRead(const String &path);

  String readFile(const String &path, size_t maxBytes);
  String getConfigValue(const String &key, const String &fallback);
  bool saveConfig(const String &systemName,
                  const String &workspaceName,
                  const String &workspaceType,
                  const String &apSsid,
                  const String &apPassword,
                  const String &defaultMode);

  uint32_t countDataLines(const String &path);

  bool clearAttendance();

  uint16_t getUserFingerId(const String &userId);
  bool deleteUser(const String &userId);

  uint64_t totalBytes();
  uint64_t usedBytes();
}

#endif
