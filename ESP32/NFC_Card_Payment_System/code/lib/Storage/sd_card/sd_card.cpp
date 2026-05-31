#include <Arduino.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <LittleFS.h>
#include "sd_card.h"

namespace
{
  fs::FS *activeFS = nullptr;
  sd_card::StorageBackend backend = sd_card::BACKEND_NONE;

  bool ready = false;
  uint8_t cs = 255;

  void createBaseFolders()
  {
    if (!ready || activeFS == nullptr)
    {
      return;
    }

    if (!activeFS->exists("/db"))
    {
      activeFS->mkdir("/db");
    }

    if (!activeFS->exists("/logs"))
    {
      activeFS->mkdir("/logs");
    }
  }

  bool startInternalStorage()
  {
    if (!LittleFS.begin(true))
    {
      return false;
    }

    activeFS = &LittleFS;
    backend = sd_card::BACKEND_INTERNAL;
    ready = true;
    createBaseFolders();
    return true;
  }
}

namespace sd_card
{
  void begin(uint8_t csPin, uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin)
  {
    ready = false;
    activeFS = nullptr;
    backend = BACKEND_NONE;
    cs = csPin;

    if (csPin != 255)
    {
      SPI.begin(sckPin, misoPin, mosiPin, csPin);
      pinMode(csPin, OUTPUT);
      digitalWrite(csPin, HIGH);

      if (SD.begin(csPin, SPI))
      {
        activeFS = &SD;
        backend = BACKEND_SD;
        ready = true;
        createBaseFolders();
        return;
      }
    }

    startInternalStorage();
  }

  void update()
  {
  }

  bool isReady()
  {
    return ready && activeFS != nullptr;
  }

  bool exists(const char *path)
  {
    if (!isReady())
    {
      return false;
    }

    return activeFS->exists(path);
  }

  bool writeText(const char *path, const String &text)
  {
    if (!isReady())
    {
      return false;
    }

    if (activeFS->exists(path))
    {
      activeFS->remove(path);
    }

    File file = activeFS->open(path, FILE_WRITE);
    if (!file)
    {
      return false;
    }

    file.print(text);
    file.close();
    return true;
  }

  bool appendLine(const char *path, const String &line)
  {
    if (!isReady())
    {
      return false;
    }

    File file = activeFS->open(path, FILE_APPEND);

    if (!file)
    {
      file = activeFS->open(path, FILE_WRITE);
      if (!file)
      {
        return false;
      }
    }

    file.println(line);
    file.close();
    return true;
  }

  String readText(const char *path)
  {
    String content;

    if (!isReady())
    {
      return content;
    }

    File file = activeFS->open(path, FILE_READ);
    if (!file)
    {
      return content;
    }

    while (file.available())
    {
      content += (char)file.read();
    }

    file.close();
    return content;
  }

  bool removeFile(const char *path)
  {
    if (!isReady())
    {
      return false;
    }

    if (!activeFS->exists(path))
    {
      return true;
    }

    return activeFS->remove(path);
  }

  bool renameFile(const char *oldPath, const char *newPath)
  {
    if (!isReady())
    {
      return false;
    }

    if (!activeFS->exists(oldPath))
    {
      return false;
    }

    if (activeFS->exists(newPath))
    {
      activeFS->remove(newPath);
    }

    return activeFS->rename(oldPath, newPath);
  }

  StorageBackend getBackend()
  {
    return backend;
  }

  bool isUsingSD()
  {
    return backend == BACKEND_SD;
  }

  bool isUsingInternal()
  {
    return backend == BACKEND_INTERNAL;
  }

  String getBackendName()
  {
    if (backend == BACKEND_SD)
    {
      return "SD Card";
    }

    if (backend == BACKEND_INTERNAL)
    {
      return "Internal Storage";
    }

    return "No Storage";
  }
}
