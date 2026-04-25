#pragma once
#include <Arduino.h>

namespace Pins
{
  extern const uint8_t I2C_SDA;
  extern const uint8_t I2C_SCL;
  extern const uint8_t MPU_INT;

  extern const uint8_t SD_CS;
  extern const uint8_t SD_MOSI;
  extern const uint8_t SD_MISO;
  extern const uint8_t SD_SCK;

  extern const uint8_t VIBRATION_MOTOR;
  extern const uint8_t BUZZER;
  extern const uint8_t BATTERY_ADC;

  extern const uint8_t LED_1;
  extern const uint8_t LED_2;
  extern const uint8_t LED_3;
  extern const uint8_t LED_4;

  extern const uint8_t USER_BUTTON;

  void begin();

  int readPin(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}
