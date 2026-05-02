#ifndef RFID_H
#define RFID_H

#include <Arduino.h>

namespace RFID
{
  void begin();
  void update();

  bool isReady();
  bool readCard(String &uid);

  String getLastUID();
}

#endif
