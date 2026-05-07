#ifndef BUZZER_H
#define BUZZER_H
#include <Arduino.h>
namespace buzzer { void begin(uint8_t pin); void update(); void beep(uint16_t onTime=100,uint16_t offTime=100,uint8_t count=1); void alarm(bool state); bool isActive(); }
#endif
