#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

namespace Pins
{
  static const uint8_t UNUSED_PIN = 255;

  static const uint8_t I2C_SDA = 8;
  static const uint8_t I2C_SCL = 9;

  static const uint8_t SHARED_MOSI = 11;
  static const uint8_t SHARED_SCK = 12;
  static const uint8_t SHARED_MISO = 13;

  static const uint8_t RC522_SS = 10;
  static const uint8_t RC522_RST = 4;

  static const uint8_t SD_CARD_CS = 15;
  static const uint8_t BUZZER_PIN = 5;

  static const uint8_t FINGERPRINT_TX = 17;
  static const uint8_t FINGERPRINT_RX = 18;

  static const uint8_t BTN1 = 1;
  static const uint8_t BTN2 = 6;
  static const uint8_t BTN3 = 7;
  static const uint8_t BTN4 = 16;
  static const uint8_t BTN5 = 3;

  static const uint8_t BATTERY_ADC = 2;
  static constexpr float BATTERY_SCALE = 2.0f;

  static const uint8_t USB_D_MINUS = 19;
  static const uint8_t USB_D_PLUS = 20;

  static const uint8_t STATUS_LED = UNUSED_PIN;

  void begin();
  void update();

  bool valid(uint8_t gpio);

  int readPin(uint8_t gpio);
  int readAnalog(uint8_t gpio);

  void writePin(uint8_t gpio, bool value);
}

#endif
