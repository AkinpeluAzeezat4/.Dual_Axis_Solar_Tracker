#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

namespace Pins
{
  extern const uint8_t LED_PIN;

  extern const uint8_t BATTERY_ADC_PIN;

  extern const uint8_t I2C_SDA_PIN;
  extern const uint8_t I2C_SCL_PIN;

  extern const uint8_t SPI_MOSI_PIN;
  extern const uint8_t SPI_SCK_PIN;
  extern const uint8_t SPI_MISO_PIN;

  extern const uint8_t CS1_PIN;
  extern const uint8_t CS2_PIN;
  extern const uint8_t CS3_PIN;

  extern const uint8_t NFC_SS_PIN;
  extern const uint8_t NFC_RST_PIN;

  extern const uint8_t SD_CS_PIN;
  extern const uint8_t BUZZER_PIN;

  extern const uint8_t BTN_UP_PIN;
  extern const uint8_t BTN_DOWN_PIN;
  extern const uint8_t BTN_LEFT_PIN;
  extern const uint8_t BTN_RIGHT_PIN;
  extern const uint8_t BTN_OK_PIN;

  void begin();

  int readPin(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}

#endif
