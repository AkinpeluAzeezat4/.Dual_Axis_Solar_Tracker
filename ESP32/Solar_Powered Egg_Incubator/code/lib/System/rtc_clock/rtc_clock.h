#ifndef RTC_CLOCK_H
#define RTC_CLOCK_H
#include <Arduino.h>
namespace rtc_clock { void begin(); void update(); String getDateTime(); bool isReady(); }
#endif
