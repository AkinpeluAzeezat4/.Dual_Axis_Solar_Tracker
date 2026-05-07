#include <Arduino.h>
#include "Pins.h"

namespace Pins
{
  const uint8_t HEARTBEAT_LED = 2;

  const uint8_t LCD_CS = 5;
  const uint8_t LCD_CLK = 18;
  const uint8_t LCD_MOSI = 23;

  const uint8_t I2C_SDA = 21;
  const uint8_t I2C_SCL = 22;

  const uint8_t PZEM_RX = 16;
  const uint8_t PZEM_TX = 17;

  const uint8_t ENCODER_A = 26;
  const uint8_t ENCODER_B = 25;
  const uint8_t ENCODER_SW = 27;

  const uint8_t PCA9555_ADDRESS = 0x20;

  const uint8_t EXP_RELAY_1_SOURCE = 0;
  const uint8_t EXP_RELAY_1_LOAD = 1;
  const uint8_t EXP_RELAY_2_SOURCE = 2;
  const uint8_t EXP_RELAY_2_LOAD = 3;
  const uint8_t EXP_RELAY_3_SOURCE = 4;
  const uint8_t EXP_RELAY_3_LOAD = 5;
  const uint8_t EXP_RELAY_4_SOURCE = 6;
  const uint8_t EXP_RELAY_4_LOAD = 7;
  const uint8_t EXP_RELAY_5_SOURCE = 8;
  const uint8_t EXP_RELAY_5_LOAD = 9;
  const uint8_t EXP_RELAY_6_SOURCE = 10;
  const uint8_t EXP_RELAY_6_LOAD = 11;

  const uint8_t EXP_IND_LED = 12;
  const uint8_t EXP_NEPA_SENSE = 13;
  const uint8_t EXP_INVERTER_SENSE = 14;
  const uint8_t EXP_12V_EN = 15;

  const bool SOURCE_SENSE_ACTIVE_HIGH = true;
  const bool INVERTER_SENSE_ACTIVE_HIGH = true;

  void begin()
  {
    pinMode(HEARTBEAT_LED, OUTPUT);
    digitalWrite(HEARTBEAT_LED, LOW);
  }

  int readPin(uint8_t gpio)
  {
    return digitalRead(gpio);
  }

  void writePin(uint8_t gpio, bool value)
  {
    digitalWrite(gpio, value ? HIGH : LOW);
  }
}
