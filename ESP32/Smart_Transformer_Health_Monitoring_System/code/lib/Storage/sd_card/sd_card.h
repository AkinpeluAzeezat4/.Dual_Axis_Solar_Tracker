#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <FS.h>

namespace sd_card
{
  void begin();
  void update();

  void logNow();
  void logEvent(const String &eventType, const String &message);

  bool isReady();
  bool isInternalReady();
  bool isStorageReady();
  bool usingSdCard();

  const char *getBackendName();
  const char *getTransformerLogFileName();
  const char *getAnalysisLogFileName();
  const char *getEventLogFileName();

  void setLogInterval(unsigned long intervalMs);
  unsigned long getLogInterval();

  String readTail(const char *path, size_t maxBytes);
  String readFileLimited(const char *path, size_t maxBytes);
  size_t getFileSize(const char *path);
  File openRead(const char *path);
}

#endif
