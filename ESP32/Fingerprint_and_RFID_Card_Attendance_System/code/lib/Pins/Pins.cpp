#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "Pins.h"

namespace Pins
{
  void begin()
  {
    Wire.begin(I2C_SDA, I2C_SCL);
    SPI.begin(SHARED_SCK, SHARED_MISO, SHARED_MOSI);

    pinMode(RC522_RST, OUTPUT);
    digitalWrite(RC522_RST, HIGH);

    pinMode(RC522_SS, OUTPUT);
    digitalWrite(RC522_SS, HIGH);

    pinMode(SD_CARD_CS, OUTPUT);
    digitalWrite(SD_CARD_CS, HIGH);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    pinMode(BTN1, INPUT_PULLUP);
    pinMode(BTN2, INPUT_PULLUP);
    pinMode(BTN3, INPUT_PULLUP);
    pinMode(BTN4, INPUT_PULLUP);
    pinMode(BTN5, INPUT_PULLUP);

    pinMode(BATTERY_ADC, INPUT);
  }

  void update()
  {
  }

  bool valid(uint8_t gpio)
  {
    return gpio != UNUSED_PIN;
  }

  int readPin(uint8_t gpio)
  {
    if (!valid(gpio))
      return LOW;

    return digitalRead(gpio);
  }

  int readAnalog(uint8_t gpio)
  {
    if (!valid(gpio))
      return 0;

    return analogRead(gpio);
  }

  void writePin(uint8_t gpio, bool value)
  {
    if (!valid(gpio))
      return;

    digitalWrite(gpio, value ? HIGH : LOW);
  }
}
