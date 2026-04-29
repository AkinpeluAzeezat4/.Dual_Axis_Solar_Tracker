#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "NFC_module.h"

namespace
{
  MFRC522 *reader = nullptr;
  bool ready = false;

  String pendingUID;
  String lastUID;
  unsigned long lastReadAt = 0;
  const unsigned long duplicateGuardMs = 1200;

  String formatUID(MFRC522 *device)
  {
    String uid;

    for (byte i = 0; i < device->uid.size; ++i)
    {
      if (device->uid.uidByte[i] < 0x10)
      {
        uid += "0";
      }

      uid += String(device->uid.uidByte[i], HEX);
    }

    uid.toUpperCase();
    return uid;
  }
}

namespace NFC_module
{
  void begin(uint8_t ssPin, uint8_t rstPin, uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin)
  {
    SPI.begin(sckPin, misoPin, mosiPin, ssPin);

    if (reader != nullptr)
    {
      delete reader;
      reader = nullptr;
    }

    reader = new MFRC522(ssPin, rstPin);
    reader->PCD_Init();
    delay(20);

    byte version = reader->PCD_ReadRegister(MFRC522::VersionReg);
    ready = (version != 0x00 && version != 0xFF);

    pendingUID = "";
    lastUID = "";
    lastReadAt = 0;
  }

  void update()
  {
    if (!ready || reader == nullptr)
    {
      return;
    }

    if (!reader->PICC_IsNewCardPresent())
    {
      return;
    }

    if (!reader->PICC_ReadCardSerial())
    {
      return;
    }

    String uid = formatUID(reader);

    if (uid != lastUID || (millis() - lastReadAt) > duplicateGuardMs)
    {
      pendingUID = uid;
      lastUID = uid;
      lastReadAt = millis();
    }

    reader->PICC_HaltA();
    reader->PCD_StopCrypto1();
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
}
