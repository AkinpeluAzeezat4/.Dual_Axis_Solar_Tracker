#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include "NFC_module.h"

namespace
{
  Adafruit_PN532 *reader = nullptr;

  bool ready = false;
  uint32_t versionData = 0;

  uint8_t nfcSsPin = 255;
  uint8_t nfcRstPin = 255;
  uint8_t spiMosiPin = 255;
  uint8_t spiMisoPin = 255;
  uint8_t spiSckPin = 255;

  String pendingUID;
  String lastUID;

  unsigned long lastReadAt = 0;
  unsigned long lastRetryAt = 0;

  const unsigned long duplicateGuardMs = 1200;
  const unsigned long retryIntervalMs = 3000;

  void hardwareReset()
  {
    if (nfcRstPin == 255)
    {
      return;
    }

    pinMode(nfcRstPin, OUTPUT);
    digitalWrite(nfcRstPin, HIGH);
    delay(20);
    digitalWrite(nfcRstPin, LOW);
    delay(150);
    digitalWrite(nfcRstPin, HIGH);
    delay(700);
  }

  String formatUID(uint8_t *uid, uint8_t uidLength)
  {
    String text;

    for (uint8_t i = 0; i < uidLength; i++)
    {
      if (uid[i] < 0x10)
      {
        text += "0";
      }

      text += String(uid[i], HEX);
    }

    text.toUpperCase();
    return text;
  }

  void initReader()
  {
    ready = false;
    versionData = 0;

    if (nfcSsPin == 255)
    {
      return;
    }

    pinMode(nfcSsPin, OUTPUT);
    digitalWrite(nfcSsPin, HIGH);

    SPI.begin(spiSckPin, spiMisoPin, spiMosiPin, nfcSsPin);

    hardwareReset();

    if (reader != nullptr)
    {
      delete reader;
      reader = nullptr;
    }

    reader = new Adafruit_PN532(nfcSsPin, &SPI);
    reader->begin();

    delay(100);

    versionData = reader->getFirmwareVersion();

    if (versionData != 0)
    {
      reader->SAMConfig();
      ready = true;
    }
  }
}

namespace NFC_module
{
  void begin(uint8_t ssPin, uint8_t rstPin, uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin)
  {
    nfcSsPin = ssPin;
    nfcRstPin = rstPin;
    spiMosiPin = mosiPin;
    spiMisoPin = misoPin;
    spiSckPin = sckPin;

    pendingUID = "";
    lastUID = "";
    lastReadAt = 0;
    lastRetryAt = 0;

    initReader();
  }

  void retryInit()
  {
    initReader();
  }

  void update()
  {
    if (!ready || reader == nullptr)
    {
      if (millis() - lastRetryAt >= retryIntervalMs)
      {
        lastRetryAt = millis();
        initReader();
      }

      return;
    }

    uint8_t uid[7];
    uint8_t uidLength = 0;

    bool success = reader->readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);

    if (!success)
    {
      return;
    }

    String uidText = formatUID(uid, uidLength);

    if (uidText != lastUID || millis() - lastReadAt >= duplicateGuardMs)
    {
      pendingUID = uidText;
      lastUID = uidText;
      lastReadAt = millis();
    }
  }

  bool isReady()
  {
    return ready;
  }

  bool consumeUID(String &uid)
  {
    if (pendingUID.length() == 0)
    {
      return false;
    }

    uid = pendingUID;
    pendingUID = "";
    return true;
  }

  String getLastUID()
  {
    return lastUID;
  }

  uint32_t getVersion()
  {
    return versionData;
  }

  String getVersionText()
  {
    if (!ready || versionData == 0)
    {
      return "PN532 not found";
    }

    uint8_t ic = (versionData >> 24) & 0xFF;
    uint8_t ver = (versionData >> 16) & 0xFF;
    uint8_t rev = (versionData >> 8) & 0xFF;

    String text = "IC:";
    text += String(ic, HEX);
    text += " V:";
    text += String(ver);
    text += ".";
    text += String(rev);

    text.toUpperCase();
    return text;
  }
}
