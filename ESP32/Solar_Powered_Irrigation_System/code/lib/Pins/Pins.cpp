#include "Pins.h"

namespace Pins
{
  const uint8_t ENCODER_CLK = 25;
  const uint8_t ENCODER_DT = 26;
  const uint8_t ENCODER_SW = 27;
  const uint8_t ULTRASONIC_TRIG = 18;
  const uint8_t ULTRASONIC_ECHO = 19;
  const uint8_t TANK_PUMP = 33;
  const uint8_t IRRIGATION_PUMP = 16;
  const uint8_t SOIL_SENSOR = 34;
  const uint8_t BATTERY_SENSE = 35;
  const uint8_t SOLAR_SENSE = 39;
  const uint8_t DHT_DATA = 32;
  const uint8_t OLED_SDA = 21;
  const uint8_t OLED_SCL = 22;
  const uint8_t BUZZER = 23;
  const uint8_t STATUS_LED = 2;

  void begin()
  {
    pinMode(ENCODER_CLK, INPUT_PULLUP);
    pinMode(ENCODER_DT, INPUT_PULLUP);
    pinMode(ENCODER_SW, INPUT_PULLUP);

    pinMode(ULTRASONIC_TRIG, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);

    pinMode(TANK_PUMP, OUTPUT);
    pinMode(IRRIGATION_PUMP, OUTPUT);
    pinMode(BUZZER, OUTPUT);
    pinMode(STATUS_LED, OUTPUT);

    digitalWrite(ULTRASONIC_TRIG, LOW);
    digitalWrite(TANK_PUMP, LOW);
    digitalWrite(IRRIGATION_PUMP, LOW);
    digitalWrite(BUZZER, LOW);
    digitalWrite(STATUS_LED, LOW);

    analogReadResolution(12);
    analogSetPinAttenuation(SOIL_SENSOR, ADC_11db);
    analogSetPinAttenuation(BATTERY_SENSE, ADC_11db);
    analogSetPinAttenuation(SOLAR_SENSE, ADC_11db);
  }

  void update()
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
