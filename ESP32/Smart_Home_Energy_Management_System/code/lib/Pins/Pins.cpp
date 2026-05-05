#include "Pins.h"

namespace Pins
{
  const uint8_t HEARTBEAT_LED = 2;
  const uint8_t LCD_CS = 5;
  const uint8_t LCD_CLK = 18;
  const uint8_t LCD_MOSI = 23;
  const uint8_t ENCODER_A = 26;
  const uint8_t ENCODER_B = 25;
  const uint8_t ENCODER_SW = 27;
  const uint8_t RELAY_1 = 13;
  const uint8_t RELAY_2 = 14;
  const uint8_t RELAY_3 = 16;
  const uint8_t RELAY_4 = 17;
  const uint8_t RELAY_5 = 21;
  const uint8_t RELAY_6 = 22;
  const uint8_t NEPA_SENSE = 34;
  const uint8_t INVERTER_SENSE = 35;

  void begin()
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
