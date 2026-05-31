#pragma once
#include <Arduino.h>

namespace sd_card
{
  enum StorageMode
  {
    STORAGE_NONE,
    STORAGE_SD,
    STORAGE_INTERNAL
  };

  void begin();
  void update();

  bool isReady();
  bool isSdReady();
  bool isInternalReady();

  void logNow();

  String readLogFile();
  String readFullLogFile();

  bool clearLogFile();

  const char *getLogFileName();
  const char *getStorageName();

  StorageMode getStorageMode();
}

