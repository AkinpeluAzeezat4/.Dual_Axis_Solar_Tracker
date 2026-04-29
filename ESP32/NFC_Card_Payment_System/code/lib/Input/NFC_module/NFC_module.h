#ifndef NFC_MODULE_H
#define NFC_MODULE_H

#include <Arduino.h>

namespace NFC_module
{
  void begin(uint8_t ssPin, uint8_t rstPin, uint8_t mosiPin, uint8_t misoPin, uint8_t sckPin);
  void update();

  bool isReady();
  bool consumeUID(String &uid);
  String getLastUID();
}

#endif
