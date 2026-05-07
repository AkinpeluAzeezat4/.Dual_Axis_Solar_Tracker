#pragma once

namespace error_handling
{
  enum ErrorCode
  {
    NO_ERROR,
    PCA9555_ERROR,
    PZEM_ERROR,
    NO_SOURCE_ERROR,
    OVERLOAD_ERROR
  };

  void begin();
  void update();

  void setError(ErrorCode error);
  void clearError();

  bool hasError();
  ErrorCode getError();
}
