#ifndef TEMP_HUM_H
#define TEMP_HUM_H

#include <Arduino.h>
namespace temp_hum { void begin(); void update(); float getTemperature(); float getHumidity(); bool isReady(); }
#endif
