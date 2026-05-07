#pragma once

#include <Arduino.h>

namespace Pins
{
  extern const uint8_t HEARTBEAT_LED;

  extern const uint8_t LCD_CS;
  extern const uint8_t LCD_CLK;
  extern const uint8_t LCD_MOSI;

  extern const uint8_t I2C_SDA;
  extern const uint8_t I2C_SCL;

  extern const uint8_t PZEM_RX;
  extern const uint8_t PZEM_TX;

  extern const uint8_t ENCODER_A;
  extern const uint8_t ENCODER_B;
  extern const uint8_t ENCODER_SW;

  extern const uint8_t PCA9555_ADDRESS;

  extern const uint8_t EXP_RELAY_1_SOURCE;
  extern const uint8_t EXP_RELAY_1_LOAD;
  extern const uint8_t EXP_RELAY_2_SOURCE;
  extern const uint8_t EXP_RELAY_2_LOAD;
  extern const uint8_t EXP_RELAY_3_SOURCE;
  extern const uint8_t EXP_RELAY_3_LOAD;
  extern const uint8_t EXP_RELAY_4_SOURCE;
  extern const uint8_t EXP_RELAY_4_LOAD;
  extern const uint8_t EXP_RELAY_5_SOURCE;
  extern const uint8_t EXP_RELAY_5_LOAD;
  extern const uint8_t EXP_RELAY_6_SOURCE;
  extern const uint8_t EXP_RELAY_6_LOAD;

  extern const uint8_t EXP_IND_LED;
  extern const uint8_t EXP_NEPA_SENSE;
  extern const uint8_t EXP_INVERTER_SENSE;
  extern const uint8_t EXP_12V_EN;

  extern const bool SOURCE_SENSE_ACTIVE_HIGH;
  extern const bool INVERTER_SENSE_ACTIVE_HIGH;

  void begin();

  int readPin(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}
