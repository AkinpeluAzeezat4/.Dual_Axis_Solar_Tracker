#include <Arduino.h>
#include <esp_system.h>
#include "error_handling.h"

namespace error_handling
{
  static bool watchdogError = false;
  static bool codeError = false;

  void begin()
  {
    esp_reset_reason_t reason = esp_reset_reason();

    watchdogError = (reason == ESP_RST_WDT ||
                     reason == ESP_RST_TASK_WDT ||
                     reason == ESP_RST_INT_WDT);

    codeError = false;
  }

  void update()
  {
  }

  void setCodeError(bool state)
  {
    codeError = state;
  }

  bool hasError()
  {
    return watchdogError || codeError;
  }

  bool hasWatchdogError()
  {
    return watchdogError;
  }

  bool hasCodeError()
  {
    return codeError;
  }
}
