#pragma once
#include <Arduino.h>

namespace Pins
{
  extern const uint8_t HEARTBEAT_LED;
  extern const uint8_t LCD_CS;
  extern const uint8_t LCD_CLK;
  extern const uint8_t LCD_MOSI;
  extern const uint8_t ENCODER_A;
  extern const uint8_t ENCODER_B;
  extern const uint8_t ENCODER_SW;
  extern const uint8_t RELAY_1;
  extern const uint8_t RELAY_2;
  extern const uint8_t RELAY_3;
  extern const uint8_t RELAY_4;
  extern const uint8_t RELAY_5;
  extern const uint8_t RELAY_6;
  extern const uint8_t NEPA_SENSE;
  extern const uint8_t INVERTER_SENSE;

  void begin();
  int readPin(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}
