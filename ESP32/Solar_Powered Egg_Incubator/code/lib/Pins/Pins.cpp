#include <Arduino.h>
#include "Pins.h"

namespace Pins
{
  const uint8_t BATTERY_LEVEL = 34;
  const uint8_t SOLAR_LEVEL = 35;

  const uint8_t ULTRASONIC_TRIG = 27;
  const uint8_t ULTRASONIC_ECHO = 26;

  const uint8_t ENCODER_A = 32;
  const uint8_t ENCODER_B = 33;
  const uint8_t ENCODER_SW = 25;

  const uint8_t BUZZER = 4;

  const uint8_t HEATER_RELAY = 16;
  const uint8_t SPINNER_RELAY = 17;
  const uint8_t HUMIDIFIER_RELAY = 18;

  const uint8_t LCD_CS = 13;
  const uint8_t LCD_CLK = 14;
  const uint8_t LCD_MOSI = 23;

  const uint8_t I2C_SDA = 21;
  const uint8_t I2C_SCL = 22;

  void begin()
  {
    pinMode(BATTERY_LEVEL, INPUT);
    pinMode(SOLAR_LEVEL, INPUT);

    pinMode(ULTRASONIC_TRIG, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);

    pinMode(ENCODER_A, INPUT_PULLUP);
    pinMode(ENCODER_B, INPUT_PULLUP);
    pinMode(ENCODER_SW, INPUT_PULLUP);

    pinMode(BUZZER, OUTPUT);

    pinMode(HEATER_RELAY, OUTPUT);
    pinMode(SPINNER_RELAY, OUTPUT);
    pinMode(HUMIDIFIER_RELAY, OUTPUT);

    digitalWrite(ULTRASONIC_TRIG, LOW);
    digitalWrite(BUZZER, LOW);

    digitalWrite(HEATER_RELAY, HIGH);
    digitalWrite(SPINNER_RELAY, HIGH);
    digitalWrite(HUMIDIFIER_RELAY, HIGH);
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
