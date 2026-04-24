#include "error_handling.h"
#include "esp_system.h"
#include "sun_sensor/sun_sensor.h"

namespace error_handling
{
  bool watchdogError = false;
  bool codeError = false;

  void begin()
  {
    esp_reset_reason_t reason = esp_reset_reason();
    watchdogError = (reason == ESP_RST_WDT || reason == ESP_RST_TASK_WDT || reason == ESP_RST_INT_WDT);
    codeError = false;
  }

  void update()
  {
    sun_sensor::SensorData data = sun_sensor::getData();

    bool badAdc =
        data.topLeft == 0 &&
        data.topRight == 0 &&
        data.bottomLeft == 0 &&
        data.bottomRight == 0 &&
        !data.isDark;

    codeError = badAdc;
  }

  void setCodeError(bool state)
  {
    codeError = state;
  }

  bool hasError()
  {
    return watchdogError || codeError;
  }
}
