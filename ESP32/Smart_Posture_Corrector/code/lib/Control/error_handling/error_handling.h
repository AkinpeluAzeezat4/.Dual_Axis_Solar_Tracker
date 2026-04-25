#pragma once
#include <Arduino.h>

namespace error_handling
{
  enum ErrorCode
  {
    NO_ERROR,
    MPU6050_ERROR,
    SD_CARD_ERROR,
    LOW_BATTERY_ERROR
  };

  void begin();
  void update();

  void setError(ErrorCode code, bool state);
  bool hasError();
  bool hasError(ErrorCode code);

  ErrorCode getLastError();
  const char *getErrorText();
}
