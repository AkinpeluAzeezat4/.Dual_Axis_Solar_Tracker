#ifndef LCD_SCREEN_H
#define LCD_SCREEN_H
#include <Arduino.h>
namespace lcd_screen { void begin(); void update(); void setBacklight(bool state); bool isAwake(); void wake(); void sleep(); }
#endif
