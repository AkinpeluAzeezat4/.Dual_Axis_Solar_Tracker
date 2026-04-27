#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

namespace Pins
{
  extern const uint8_t BATTERY_LEVEL;
  extern const uint8_t SOLAR_LEVEL;

  extern const uint8_t ULTRASONIC_TRIG;
  extern const uint8_t ULTRASONIC_ECHO;

  extern const uint8_t ENCODER_A;
  extern const uint8_t ENCODER_B;
  extern const uint8_t ENCODER_SW;

  extern const uint8_t BUZZER;

  extern const uint8_t HEATER_RELAY;
  extern const uint8_t SPINNER_RELAY;
  extern const uint8_t HUMIDIFIER_RELAY;

  extern const uint8_t LCD_CS;
  extern const uint8_t LCD_CLK;
  extern const uint8_t LCD_MOSI;

  extern const uint8_t I2C_SDA;
  extern const uint8_t I2C_SCL;

  void begin();

  int readPin(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}

#endif
