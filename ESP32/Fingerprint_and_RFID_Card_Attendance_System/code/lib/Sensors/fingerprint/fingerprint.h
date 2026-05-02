#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <Arduino.h>

namespace fingerprint
{
  void begin();
  void update();

  bool isReady();

  bool search(uint16_t &fingerId, uint16_t &confidence);
  bool enroll(uint16_t fingerId, String &message);
  bool deleteTemplate(uint16_t fingerId, String &message);

  uint16_t getTemplateCount();
}

#endif
