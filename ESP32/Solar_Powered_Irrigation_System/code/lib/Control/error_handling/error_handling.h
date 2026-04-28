#pragma once

namespace error_handling
{
  void begin();
  void update();

  void setCodeError(bool state);
  bool hasError();
  bool hasWatchdogError();
  bool hasCodeError();
}
