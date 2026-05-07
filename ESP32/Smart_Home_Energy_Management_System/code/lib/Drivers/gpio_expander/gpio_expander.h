#pragma once

#include <Arduino.h>

namespace gpio_expander
{
  void begin();
  void begin(uint8_t address);
  void begin(uint8_t address, int sdaPin, int sclPin);

  void update();

  bool isReady();

  void pinMode(uint8_t pin, uint8_t mode);
  void digitalWrite(uint8_t pin, bool state);
  bool digitalRead(uint8_t pin);

  void writePort(uint16_t value);
  uint16_t readPort();

  void setAllInputs();
  void setAllOutputs();

  uint16_t getOutputState();
  uint16_t getInputState();
  uint16_t getConfigState();
}
