#include <Arduino.h>
#include "UART.h"
#include "Pins.h"

namespace UART
{
  static HardwareSerial motorSerial(2);
  static const uint32_t baudRate = 115200;

  void begin()
  {
    motorSerial.begin(baudRate, SERIAL_8N1, Pins::MOTOR_RX, Pins::MOTOR_TX);
  }

  void update()
  {
  }

  void sendByte(uint8_t data)
  {
    motorSerial.write(data);
  }

  void sendBuffer(const uint8_t *data, size_t length)
  {
    if (data == nullptr || length == 0)
      return;
    motorSerial.write(data, length);
  }

  bool available()
  {
    return motorSerial.available() > 0;
  }

  int read()
  {
    if (!available())
      return -1;
    return motorSerial.read();
  }
}
