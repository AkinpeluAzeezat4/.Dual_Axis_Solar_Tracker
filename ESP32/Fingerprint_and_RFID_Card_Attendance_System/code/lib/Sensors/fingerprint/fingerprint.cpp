#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include "fingerprint.h"
#include "Pins.h"

namespace fingerprint
{
  static HardwareSerial fingerSerial(1);
  static Adafruit_Fingerprint sensor(&fingerSerial);

  static bool ready = false;
  static uint16_t templateCount = 0;

  static bool waitForFingerImage(uint32_t timeoutMs, String &message)
  {
    unsigned long start = millis();

    while (millis() - start < timeoutMs)
    {
      uint8_t p = sensor.getImage();

      if (p == FINGERPRINT_OK)
        return true;

      if (p == FINGERPRINT_PACKETRECIEVEERR)
      {
        message = "Fingerprint comm error";
        return false;
      }

      if (p == FINGERPRINT_IMAGEFAIL)
      {
        message = "Fingerprint image error";
        return false;
      }

      delay(40);
      yield();
    }

    message = "Fingerprint timeout";
    return false;
  }

  static void waitForFingerRemove(uint32_t timeoutMs)
  {
    unsigned long start = millis();

    while (millis() - start < timeoutMs)
    {
      if (sensor.getImage() == FINGERPRINT_NOFINGER)
        return;

      delay(60);
      yield();
    }
  }

  void begin()
  {
    ready = false;
    templateCount = 0;

    fingerSerial.begin(
        57600,
        SERIAL_8N1,
        Pins::FINGERPRINT_RX,
        Pins::FINGERPRINT_TX);

    sensor.begin(57600);

    ready = sensor.verifyPassword();

    if (ready)
    {
      sensor.getTemplateCount();
      templateCount = sensor.templateCount;
    }
  }

  void update()
  {
  }

  bool isReady()
  {
    return ready;
  }

  bool search(uint16_t &fingerId, uint16_t &confidence)
  {
    fingerId = 0;
    confidence = 0;

    if (!ready)
      return false;

    uint8_t p = sensor.getImage();

    if (p != FINGERPRINT_OK)
      return false;

    p = sensor.image2Tz();

    if (p != FINGERPRINT_OK)
      return false;

    p = sensor.fingerFastSearch();

    if (p != FINGERPRINT_OK)
      return false;

    fingerId = sensor.fingerID;
    confidence = sensor.confidence;

    return true;
  }

  bool enroll(uint16_t fingerId, String &message)
  {
    if (!ready)
    {
      message = "Fingerprint not ready";
      return false;
    }

    if (fingerId < 1 || fingerId > 200)
    {
      message = "Invalid finger ID";
      return false;
    }

    message = "Place finger";

    if (!waitForFingerImage(15000, message))
      return false;

    uint8_t p = sensor.image2Tz(1);

    if (p != FINGERPRINT_OK)
    {
      message = "First scan failed";
      return false;
    }

    message = "Remove finger";
    waitForFingerRemove(8000);
    delay(600);

    message = "Place same finger";

    if (!waitForFingerImage(15000, message))
      return false;

    p = sensor.image2Tz(2);

    if (p != FINGERPRINT_OK)
    {
      message = "Second scan failed";
      return false;
    }

    p = sensor.createModel();

    if (p != FINGERPRINT_OK)
    {
      message = "Finger mismatch";
      return false;
    }

    p = sensor.storeModel(fingerId);

    if (p != FINGERPRINT_OK)
    {
      message = "Template save failed";
      return false;
    }

    sensor.getTemplateCount();
    templateCount = sensor.templateCount;

    message = "Fingerprint saved";
    return true;
  }

  bool deleteTemplate(uint16_t fingerId, String &message)
  {
    if (!ready)
    {
      message = "Fingerprint not ready";
      return false;
    }

    if (fingerId < 1 || fingerId > 200)
    {
      message = "Invalid finger ID";
      return false;
    }

    uint8_t p = sensor.deleteModel(fingerId);

    if (p != FINGERPRINT_OK)
    {
      message = "Fingerprint delete failed";
      return false;
    }

    sensor.getTemplateCount();
    templateCount = sensor.templateCount;

    message = "Fingerprint deleted";
    return true;
  }

  uint16_t getTemplateCount()
  {
    return templateCount;
  }
}
