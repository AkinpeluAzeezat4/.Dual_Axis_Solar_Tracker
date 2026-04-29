#include "Pins.h"

namespace Pins
{
  const uint8_t AZ_EN = 25;
  const uint8_t AZ_IN1 = 16;
  const uint8_t AZ_IN2 = 17;

  const uint8_t EL_EN = 26;
  const uint8_t EL_IN1 = 18;
  const uint8_t EL_IN2 = 19;

  const uint8_t LDR_TOP_LEFT = 32;
  const uint8_t LDR_TOP_RIGHT = 33;
  const uint8_t LDR_BOTTOM_LEFT = 34;
  const uint8_t LDR_BOTTOM_RIGHT = 35;
  const uint8_t LDR_NIGHT = 36;
  const uint8_t POT_CALIBRATION = 39;

  const uint8_t ENC_CLK = 21;
  const uint8_t ENC_DT = 22;
  const uint8_t ENC_SW = 23;

  const uint8_t LED_LEFT = 4;
  const uint8_t LED_RIGHT = 5;
  const uint8_t LED_UP = 14;
  const uint8_t LED_DOWN = 27;
  const uint8_t LED_STATUS = 2;

  void begin()
  {
    pinMode(AZ_EN, OUTPUT);
    pinMode(AZ_IN1, OUTPUT);
    pinMode(AZ_IN2, OUTPUT);

    pinMode(EL_EN, OUTPUT);
    pinMode(EL_IN1, OUTPUT);
    pinMode(EL_IN2, OUTPUT);

    pinMode(ENC_CLK, INPUT_PULLUP);
    pinMode(ENC_DT, INPUT_PULLUP);
    pinMode(ENC_SW, INPUT_PULLUP);

    pinMode(LED_LEFT, OUTPUT);
    pinMode(LED_RIGHT, OUTPUT);
    pinMode(LED_UP, OUTPUT);
    pinMode(LED_DOWN, OUTPUT);
    pinMode(LED_STATUS, OUTPUT);

    digitalWrite(AZ_EN, LOW);
    digitalWrite(AZ_IN1, LOW);
    digitalWrite(AZ_IN2, LOW);

    digitalWrite(EL_EN, LOW);
    digitalWrite(EL_IN1, LOW);
    digitalWrite(EL_IN2, LOW);

    digitalWrite(LED_LEFT, LOW);
    digitalWrite(LED_RIGHT, LOW);
    digitalWrite(LED_UP, LOW);
    digitalWrite(LED_DOWN, LOW);
    digitalWrite(LED_STATUS, LOW);

    analogReadResolution(12);

    analogSetPinAttenuation(LDR_TOP_LEFT, ADC_11db);
    analogSetPinAttenuation(LDR_TOP_RIGHT, ADC_11db);
    analogSetPinAttenuation(LDR_BOTTOM_LEFT, ADC_11db);
    analogSetPinAttenuation(LDR_BOTTOM_RIGHT, ADC_11db);
    analogSetPinAttenuation(LDR_NIGHT, ADC_11db);
    analogSetPinAttenuation(POT_CALIBRATION, ADC_11db);
  }

  int readPin(uint8_t gpio)
  {
    return digitalRead(gpio);
  }

  int readAnalog(uint8_t gpio)
  {
    return analogRead(gpio);
  }

  void writePin(uint8_t gpio, bool value)
  {
    digitalWrite(gpio, value ? HIGH : LOW);
  }
}
