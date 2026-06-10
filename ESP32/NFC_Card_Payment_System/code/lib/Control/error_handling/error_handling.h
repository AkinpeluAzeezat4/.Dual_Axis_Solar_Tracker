#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <Arduino.h>

namespace error_handling
{
  void begin();
  void update();

  void setSdError(bool state);
  void setNfcError(bool state);
  void setRtcError(bool state);
  void setCodeError(bool state);
  void setBatteryError(bool state);

  bool hasError();
  String getFirstErrorText();
}

#endif
