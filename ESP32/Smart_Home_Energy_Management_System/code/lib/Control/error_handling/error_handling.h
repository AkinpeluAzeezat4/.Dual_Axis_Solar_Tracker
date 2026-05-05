#pragma once

namespace error_handling
{
  enum ErrorCode
  {
    NO_ERROR,
    PHCN_UNAVAILABLE,
    INVERTER_UNAVAILABLE,
    OVERLOAD,
    SENSOR_ERROR,
    RELAY_ERROR
  };

  void begin();
  void update();

  void setError(ErrorCode error);
  void clearError();
  bool hasError();
  ErrorCode getError();
}
