#ifndef SLEEP_WAKE_H
#define SLEEP_WAKE_H

#include <Arduino.h>
namespace sleep_wake { void begin(); void update(); void requestSleep(bool state); bool wokeFromSleep(); }
#endif
