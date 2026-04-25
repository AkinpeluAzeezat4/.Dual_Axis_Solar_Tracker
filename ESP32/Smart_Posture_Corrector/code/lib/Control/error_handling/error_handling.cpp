#include <Arduino.h>
#include "error_handling.h"
#include "mpu6050_sensor.h"
#include "sd_card.h"
#include "battery_level.h"
#include "led_indicator.h"
#include "buzzer.h"

namespace error_handling
{
  static bool mpuError = false;
  static bool sdError = false;
  static bool lowBatteryError = false;

  static ErrorCode lastError = NO_ERROR;

  static void refreshLastError()
  {
    if (mpuError)
      lastError = MPU6050_ERROR;
    else if (sdError)
      lastError = SD_CARD_ERROR;
    else if (lowBatteryError)
      lastError = LOW_BATTERY_ERROR;
    else
      lastError = NO_ERROR;
  }

  void begin()
  {
    mpuError = false;
    sdError = false;
    lowBatteryError = false;
    lastError = NO_ERROR;
  }

  void update()
  {
    mpuError = !mpu6050_sensor::isReady();
    sdError = !sd_card::isReady();
    lowBatteryError = battery_level::isLow();

    refreshLastError();

    led_indicator::setFault(hasError());

    if (mpuError || lowBatteryError)
    {
      buzzer::setAlert(true);
    }
    else
    {
      buzzer::setAlert(false);
    }
  }

  void setError(ErrorCode code, bool state)
  {
    if (code == MPU6050_ERROR)
      mpuError = state;
    else if (code == SD_CARD_ERROR)
      sdError = state;
    else if (code == LOW_BATTERY_ERROR)
      lowBatteryError = state;

    refreshLastError();
  }

  bool hasError()
  {
    return mpuError || sdError || lowBatteryError;
  }

  bool hasError(ErrorCode code)
  {
    if (code == MPU6050_ERROR)
      return mpuError;
    if (code == SD_CARD_ERROR)
      return sdError;
    if (code == LOW_BATTERY_ERROR)
      return lowBatteryError;
    return false;
  }

  ErrorCode getLastError()
  {
    return lastError;
  }

  const char *getErrorText()
  {
    switch (lastError)
    {
    case NO_ERROR:
      return "NO_ERROR";
    case MPU6050_ERROR:
      return "MPU6050_ERROR";
    case SD_CARD_ERROR:
      return "SD_CARD_ERROR";
    case LOW_BATTERY_ERROR:
      return "LOW_BATTERY";
    default:
      return "UNKNOWN_ERROR";
    }
  }
}
