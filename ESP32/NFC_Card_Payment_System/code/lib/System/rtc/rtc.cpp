#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "rtc.h"

namespace
{
  RTC_DS3231 rtcChip;
  bool ready = false;
  String cachedTimestamp = "RTC not ready";
  unsigned long lastRefresh = 0;

  String twoDigits(int value)
  {
    String text = String(value);

    if (value < 10)
    {
      text = "0" + text;
    }

    return text;
  }
}

namespace rtc
{
  void begin()
  {
    ready = rtcChip.begin();

    if (ready && rtcChip.lostPower())
    {
      rtcChip.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    update();
  }

  void update()
  {
    if (!ready)
    {
      cachedTimestamp = "RTC not ready";
      return;
    }

    if (millis() - lastRefresh < 250)
    {
      return;
    }

    lastRefresh = millis();

    DateTime now = rtcChip.now();
    cachedTimestamp =
        String(now.year()) + "-" +
        twoDigits(now.month()) + "-" +
        twoDigits(now.day()) + " " +
        twoDigits(now.hour()) + ":" +
        twoDigits(now.minute()) + ":" +
        twoDigits(now.second());
  }

  bool isReady()
  {
    return ready;
  }

  String getTimestamp()
  {
    return cachedTimestamp;
  }
}
