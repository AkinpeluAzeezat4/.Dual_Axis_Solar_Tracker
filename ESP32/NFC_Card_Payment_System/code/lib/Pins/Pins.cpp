#include <Arduino.h>
#include "Pins.h"

namespace Pins
{
  const uint8_t LED_PIN = 48;

  const uint8_t BATTERY_ADC_PIN = 1;

  const uint8_t I2C_SDA_PIN = 8;
  const uint8_t I2C_SCL_PIN = 9;

  const uint8_t SPI_MOSI_PIN = 11;
  const uint8_t SPI_SCK_PIN = 12;
  const uint8_t SPI_MISO_PIN = 13;

  const uint8_t CS1_PIN = 10;
  const uint8_t CS2_PIN = 15;
  const uint8_t CS3_PIN = 5;

  const uint8_t NFC_SS_PIN = CS3_PIN;
  const uint8_t NFC_RST_PIN = 4;

  const uint8_t SD_CS_PIN = CS1_PIN;
  const uint8_t BUZZER_PIN = CS2_PIN;

  const uint8_t BTN_UP_PIN = 2;
  const uint8_t BTN_DOWN_PIN = 6;
  const uint8_t BTN_LEFT_PIN = 7;
  const uint8_t BTN_RIGHT_PIN = 16;
  const uint8_t BTN_OK_PIN = 3;

  void begin()
  {
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
