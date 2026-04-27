#ifndef UART_H
#define UART_H

#include <Arduino.h>

namespace UART
{
  void begin();
  void update();

  void sendByte(uint8_t data);
  void sendBuffer(const uint8_t *data, size_t length);
  bool available();
  int read();
}

#endif
