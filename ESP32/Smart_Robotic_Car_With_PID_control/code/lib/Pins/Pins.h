#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

namespace Pins
{
  extern const uint8_t MOTOR_UART_RX;
  extern const uint8_t MOTOR_UART_TX;

  extern const uint8_t I2C_SDA;
  extern const uint8_t I2C_SCL;
  extern const uint8_t MPU_INT;

  extern const uint8_t ULTRASONIC_TRIG;
  extern const uint8_t ULTRASONIC_ECHO;
  extern const uint8_t SERVO_SIGNAL;

  extern const uint8_t BATTERY_ADC;
  extern const uint8_t BATTERY_LED1;
  extern const uint8_t BATTERY_LED2;
  extern const uint8_t BATTERY_LED3;
  extern const uint8_t BATTERY_LED4;

  extern const uint8_t POWER_LED;

  void begin();
  void update();

  int readPin(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}

#endif
