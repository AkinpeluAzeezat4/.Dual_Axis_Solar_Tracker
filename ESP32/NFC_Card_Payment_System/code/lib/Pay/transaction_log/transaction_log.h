#ifndef TRANSACTION_LOG_H
#define TRANSACTION_LOG_H

#include <Arduino.h>

namespace transaction_log
{
  void begin();
  bool log(const String &uid, const String &name, long amount, long balanceAfter, const String &status);
}

#endif
