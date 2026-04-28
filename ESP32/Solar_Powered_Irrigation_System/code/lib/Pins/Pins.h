#pragma once
#include <Arduino.h>

namespace Pins
{
  extern const uint8_t ENCODER_CLK;
  extern const uint8_t ENCODER_DT;
  extern const uint8_t ENCODER_SW;
  extern const uint8_t ULTRASONIC_TRIG;
  extern const uint8_t ULTRASONIC_ECHO;
  extern const uint8_t TANK_PUMP;
  extern const uint8_t IRRIGATION_PUMP;
  extern const uint8_t SOIL_SENSOR;
  extern const uint8_t BATTERY_SENSE;
  extern const uint8_t SOLAR_SENSE;
  extern const uint8_t DHT_DATA;
  extern const uint8_t OLED_SDA;
  extern const uint8_t OLED_SCL;
  extern const uint8_t BUZZER;
  extern const uint8_t STATUS_LED;

  void begin();
  void update();
  int readPin(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}
