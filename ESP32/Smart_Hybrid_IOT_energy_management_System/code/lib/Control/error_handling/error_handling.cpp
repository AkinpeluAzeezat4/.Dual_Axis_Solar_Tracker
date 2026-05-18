#include <Arduino.h>
#include "error_handling.h"
#include "gpio_expander/gpio_expander.h"
#include "pzem_sensor/pzem_sensor.h"
#include "nepa_sense/nepa_sense.h"
#include "inverter_sense/inverter_sense.h"

namespace error_handling
{
  ErrorCode currentError = NO_ERROR;

  void begin()
  {
    currentError = NO_ERROR;
  }

  void update()
  {
    if (!gpio_expander::isReady())
    {
      currentError = PCA9555_ERROR;
      return;
    }

    if (!nepa_sense::isAvailable() && !inverter_sense::isAvailable())
    {
      currentError = NO_SOURCE_ERROR;
      return;
    }

    if (!pzem_sensor::hasValidData())
    {
      currentError = PZEM_ERROR;
      return;
    }

    currentError = NO_ERROR;
  }

  void setError(ErrorCode error)
  {
    currentError = error;
  }

  void clearError()
  {
    currentError = NO_ERROR;
  }

  bool hasError()
  {
    return currentError != NO_ERROR;
  }

  ErrorCode getError()
  {
    return currentError;
  }
}
