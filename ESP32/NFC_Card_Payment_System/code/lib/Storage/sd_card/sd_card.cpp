#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "sd_card.h"

namespace
{
  bool ready = false;
  uint8_t cs = 255;
}

namespace sd_card
{
  void begin(uint8_t csPin, uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin)
  {
    ready = false;
    cs = csPin;

    if (csPin == 255)
    {
      return;
    }

    SPI.begin(sckPin, misoPin, mosiPin, csPin);
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);

    ready = SD.begin(csPin, SPI);

    if (ready)
    {
      SD.mkdir("/db");
      SD.mkdir("/logs");
    }
  }

  void update()
  {
  }

  bool isReady()
  {
    return ready;
  }

  bool exists(const char *path)
  {
    if (!ready)
      return false;

    return SD.exists(path);
  }

  bool writeText(const char *path, const String &text)
  {
    if (!ready)
      return false;

    File file = SD.open(path, FILE_WRITE);
    if (!file)
      return false;

    file.seek(0);
    file.print(text);
    file.truncate(file.position());
    file.close();
    return true;
  }

  bool appendLine(const char *path, const String &line)
  {
    if (!ready)
      return false;

    File file = SD.open(path, FILE_APPEND);
    if (!file)
    {
      file = SD.open(path, FILE_WRITE);
      if (!file)
        return false;
    }

    file.println(line);
    file.close();
    return true;
  }

  String readText(const char *path)
  {
    String content;

    if (!ready)
      return content;

    File file = SD.open(path, FILE_READ);
    if (!file)
      return content;

    while (file.available())
    {
      content += (char)file.read();
    }

    file.close();
    return content;
  }

  bool removeFile(const char *path)
  {
    if (!ready)
      return false;

    if (!SD.exists(path))
      return true;

    return SD.remove(path);
  }
}
