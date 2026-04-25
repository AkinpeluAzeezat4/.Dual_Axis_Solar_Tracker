#include <Arduino.h>
#include "Pins.h"

namespace Pins
{
  const uint8_t I2C_SDA = 21;
  const uint8_t I2C_SCL = 22;
  const uint8_t MPU_INT = 27;

  const uint8_t SD_CS = 5;
  const uint8_t SD_MOSI = 23;
  const uint8_t SD_MISO = 19;
  const uint8_t SD_SCK = 18;

  const uint8_t VIBRATION_MOTOR = 25;
  const uint8_t BUZZER = 26;

  const uint8_t BATTERY_ADC = 34;

  const uint8_t LED_1 = 13;
  const uint8_t LED_2 = 14;
  const uint8_t LED_3 = 16;
  const uint8_t LED_4 = 17;

  const uint8_t USER_BUTTON = 32;

  void begin()
  {
    pinMode(MPU_INT, INPUT);

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);

    pinMode(VIBRATION_MOTOR, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(LED_3, OUTPUT);
    pinMode(LED_4, OUTPUT);

    pinMode(USER_BUTTON, INPUT_PULLUP);
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
