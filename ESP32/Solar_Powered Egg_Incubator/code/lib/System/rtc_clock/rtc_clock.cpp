#include <Arduino.h>
#include <RTClib.h>
#include "rtc_clock.h"

namespace rtc_clock
{
  static RTC_DS3231 rtc;
  static bool ready = false;
  static String dateTime = "RTC not found";
  static unsigned long lastRead = 0;

  void begin()
  {
    ready = rtc.begin();

    if (ready && rtc.lostPower())
    {
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
  }

  void update()
  {
    if (!ready)
    {
      return;
    }

    if (millis() - lastRead < 1000)
    {
      return;
    }

    lastRead = millis();

    DateTime now = rtc.now();

    char buffer[24];

    snprintf(
        buffer,
        sizeof(buffer),
        "%04d-%02d-%02d %02d:%02d:%02d",
        now.year(),
        now.month(),
        now.day(),
        now.hour(),
        now.minute(),
        now.second());

    dateTime = buffer;
  }

  String getDateTime()
  {
    return dateTime;
  }

  bool isReady()
  {
    return ready;
  }
}
