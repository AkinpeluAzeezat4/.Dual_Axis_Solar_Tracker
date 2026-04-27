#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include "yahboom_motor.h"
#include "Pins.h"

namespace yahboom_motor
{
  static HardwareSerial motorSerial(2);
  static MotorData data;
  static char rxBuffer[96];
  static char cleanBuffer[96];
  static uint8_t rxIndex = 0;
  static bool receiving = false;
  static unsigned long lastCommand = 0;

  static int16_t clampSpeed(int16_t value)
  {
    if (value > 1000)
      return 1000;
    if (value < -1000)
      return -1000;
    return value;
  }

  static void sendCommand(const char *command)
  {
    motorSerial.print(command);
    lastCommand = millis();
  }

  static void parseFourLongs(const char *text, long *out)
  {
    char temp[72];
    strncpy(temp, text, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    char *token = strtok(temp, ",");

    for (uint8_t i = 0; i < 4 && token != nullptr; i++)
    {
      out[i] = atol(token);
      token = strtok(nullptr, ",");
    }
  }

  static void parseFourFloats(const char *text, float *out)
  {
    char temp[72];
    strncpy(temp, text, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';

    char *token = strtok(temp, ",");

    for (uint8_t i = 0; i < 4 && token != nullptr; i++)
    {
      out[i] = atof(token);
      token = strtok(nullptr, ",");
    }
  }

  static void parseFrame(const char *frame)
  {
    if (strncmp(frame, "MAll:", 5) == 0)
    {
      parseFourLongs(frame + 5, data.encoderTotal);
      data.hasNewFrame = true;
      return;
    }

    if (strncmp(frame, "MTEP:", 5) == 0)
    {
      parseFourLongs(frame + 5, data.encoderOffset);
      data.hasNewFrame = true;
      return;
    }

    if (strncmp(frame, "MSPD:", 5) == 0)
    {
      parseFourFloats(frame + 5, data.speed);
      data.hasNewFrame = true;
      return;
    }
  }

  static void readSerial()
  {
    while (motorSerial.available())
    {
      char c = (char)motorSerial.read();

      if (c == '$')
      {
        receiving = true;
        rxIndex = 0;
        memset(rxBuffer, 0, sizeof(rxBuffer));
        continue;
      }

      if (!receiving)
        continue;

      if (c == '#')
      {
        rxBuffer[rxIndex] = '\0';
        strncpy(cleanBuffer, rxBuffer, sizeof(cleanBuffer) - 1);
        cleanBuffer[sizeof(cleanBuffer) - 1] = '\0';

        receiving = false;
        rxIndex = 0;

        parseFrame(cleanBuffer);
        continue;
      }

      if (rxIndex < sizeof(rxBuffer) - 1)
      {
        rxBuffer[rxIndex++] = c;
      }
      else
      {
        receiving = false;
        rxIndex = 0;
      }
    }
  }

  void begin()
  {
    motorSerial.begin(115200, SERIAL_8N1, Pins::MOTOR_UART_RX, Pins::MOTOR_UART_TX);

    memset(&data, 0, sizeof(data));
    data.ready = true;

    delay(300);

    configureL520();
    stop();
  }

  void update()
  {
    readSerial();
  }

  void configureL520()
  {
    sendCommand("$mtype:1#");
    delay(100);
    sendCommand("$mphase:40#");
    delay(100);
    sendCommand("$mline:11#");
    delay(100);
    sendCommand("$wdiameter:67.000#");
    delay(100);
    sendCommand("$deadzone:1900#");
    delay(100);
  }

  void setSpeed(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
  {
    char command[64];

    snprintf(
        command,
        sizeof(command),
        "$spd:%d,%d,%d,%d#",
        clampSpeed(m1),
        clampSpeed(m2),
        clampSpeed(m3),
        clampSpeed(m4));

    sendCommand(command);
  }

  void setPWM(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
  {
    char command[64];

    snprintf(
        command,
        sizeof(command),
        "$pwm:%d,%d,%d,%d#",
        clampSpeed(m1),
        clampSpeed(m2),
        clampSpeed(m3),
        clampSpeed(m4));

    sendCommand(command);
  }

  void stop()
  {
    setSpeed(0, 0, 0, 0);
  }

  void forward(int16_t speed)
  {
    speed = abs(clampSpeed(speed));
    setSpeed(speed, speed, speed, speed);
  }

  void backward(int16_t speed)
  {
    speed = abs(clampSpeed(speed));
    setSpeed(-speed, -speed, -speed, -speed);
  }

  void turnLeft(int16_t speed)
  {
    speed = abs(clampSpeed(speed));
    setSpeed(speed / 3, speed / 3, speed, speed);
  }

  void turnRight(int16_t speed)
  {
    speed = abs(clampSpeed(speed));
    setSpeed(speed, speed, speed / 3, speed / 3);
  }

  void rotateLeft(int16_t speed)
  {
    speed = abs(clampSpeed(speed));
    setSpeed(-speed, -speed, speed, speed);
  }

  void rotateRight(int16_t speed)
  {
    speed = abs(clampSpeed(speed));
    setSpeed(speed, speed, -speed, -speed);
  }

  MotorData getData()
  {
    MotorData copy = data;
    data.hasNewFrame = false;
    return copy;
  }

  bool isReady()
  {
    return data.ready;
  }
}
