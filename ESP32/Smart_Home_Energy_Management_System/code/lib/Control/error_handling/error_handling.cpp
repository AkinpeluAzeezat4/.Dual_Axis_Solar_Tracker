#include <Arduino.h>
#include "error_handling.h"

namespace error_handling
{
  ErrorCode currentError = NO_ERROR;

  void begin()
  {
    currentError = NO_ERROR;
  }

  void update()
  {
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
