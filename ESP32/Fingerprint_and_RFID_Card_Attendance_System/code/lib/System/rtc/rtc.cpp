#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include "rtc.h"
#include "Pins.h"

namespace rtc
{
  static RTC_DS3231 rtcChip;
  static bool ready = false;
  static String cachedTimestamp = "0000-00-00 00:00:00";
  static unsigned long lastRefresh = 0;
  static const unsigned long refreshInterval = 500;

  static String twoDigits(int value)
  {
    if (value < 10)
      return "0" + String(value);

    return String(value);
  }

  static String millisTimestamp()
  {
    unsigned long seconds = millis() / 1000;
    unsigned long h = (seconds / 3600) % 24;
    unsigned long m = (seconds / 60) % 60;
    unsigned long s = seconds % 60;

    return "PROTO-0000 " + twoDigits((int)h) + ":" + twoDigits((int)m) + ":" + twoDigits((int)s);
  }

  static void refreshNow()
  {
    if (!ready)
    {
      cachedTimestamp = millisTimestamp();
      return;
    }

    DateTime now = rtcChip.now();

    cachedTimestamp =
        String(now.year()) + "-" +
        twoDigits(now.month()) + "-" +
        twoDigits(now.day()) + " " +
        twoDigits(now.hour()) + ":" +
        twoDigits(now.minute()) + ":" +
        twoDigits(now.second());
  }

  void begin()
  {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);

    ready = rtcChip.begin();

    if (ready && rtcChip.lostPower())
      rtcChip.adjust(DateTime(F(__DATE__), F(__TIME__)));

    refreshNow();
    lastRefresh = millis();
  }

  void update()
  {
    if (millis() - lastRefresh < refreshInterval)
      return;

    lastRefresh = millis();
    refreshNow();
  }

  bool isReady()
  {
    return ready;
  }

  String getTimestamp()
  {
    return cachedTimestamp;
  }

  String getDate()
  {
    int split = cachedTimestamp.indexOf(' ');

    if (split < 0)
      return cachedTimestamp;

    return cachedTimestamp.substring(0, split);
  }

  String getTime()
  {
    int split = cachedTimestamp.indexOf(' ');

    if (split < 0)
      return "00:00:00";

    return cachedTimestamp.substring(split + 1);
  }
}
