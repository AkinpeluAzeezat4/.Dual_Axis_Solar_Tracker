#ifndef RTC_H
#define RTC_H

#include <Arduino.h>

namespace rtc
{
  void begin();
  void update();

  bool isReady();

  String getTimestamp();
  String getDate();
  String getTime();
}

#endif
