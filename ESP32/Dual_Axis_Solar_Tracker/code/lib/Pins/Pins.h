#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

namespace Pins
{
  extern const uint8_t AZ_EN;
  extern const uint8_t AZ_IN1;
  extern const uint8_t AZ_IN2;

  extern const uint8_t EL_EN;
  extern const uint8_t EL_IN1;
  extern const uint8_t EL_IN2;

  extern const uint8_t LDR_TOP_LEFT;
  extern const uint8_t LDR_TOP_RIGHT;
  extern const uint8_t LDR_BOTTOM_LEFT;
  extern const uint8_t LDR_BOTTOM_RIGHT;
  extern const uint8_t LDR_NIGHT;
  extern const uint8_t POT_CALIBRATION;

  extern const uint8_t ENC_CLK;
  extern const uint8_t ENC_DT;
  extern const uint8_t ENC_SW;

  extern const uint8_t LED_LEFT;
  extern const uint8_t LED_RIGHT;
  extern const uint8_t LED_UP;
  extern const uint8_t LED_DOWN;
  extern const uint8_t LED_STATUS;

  void begin();

  int readPin(uint8_t gpio);
  int readAnalog(uint8_t gpio);
  void writePin(uint8_t gpio, bool value);
}

#endif
