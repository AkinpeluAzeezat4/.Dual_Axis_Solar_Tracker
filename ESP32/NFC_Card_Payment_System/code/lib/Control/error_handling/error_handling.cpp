#include <Arduino.h>
#include "error_handling.h"

namespace
{
  bool sdError = false;
  bool nfcError = false;
  bool rtcError = false;
  bool codeError = false;
  bool batteryError = false;
}

namespace error_handling
{
  void begin()
  {
    sdError = false;
    nfcError = false;
    rtcError = false;
    codeError = false;
    batteryError = false;
  }

  void update()
  {
  }

  void setSdError(bool state)
  {
    sdError = state;
  }

  void setNfcError(bool state)
  {
    nfcError = state;
  }

  void setRtcError(bool state)
  {
    rtcError = state;
  }

  void setCodeError(bool state)
  {
    codeError = state;
  }

  void setBatteryError(bool state)
  {
    batteryError = state;
  }

  bool hasError()
  {
    return sdError || nfcError || rtcError || codeError || batteryError;
  }

  String getFirstErrorText()
  {
    if (batteryError)
      return "Low Battery";

    if (sdError)
      return "SD Card Fault";

    if (nfcError)
      return "NFC Fault";

    if (rtcError)
      return "RTC Fault";

    if (codeError)
      return "System Fault";

    return "";
  }
}
