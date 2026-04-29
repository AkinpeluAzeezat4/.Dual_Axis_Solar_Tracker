#include <Arduino.h>
#include "transaction_log.h"
#include "sd_card/sd_card.h"
#include "rtc/rtc.h"

namespace
{
  const char *logPath = "/logs/transactions.csv";
}

namespace transaction_log
{
  void begin()
  {
    if (!sd_card::isReady())
    {
      return;
    }

    if (!sd_card::exists(logPath))
    {
      sd_card::appendLine(logPath, "timestamp,uid,name,amount,balance_after,status");
    }
  }

  bool log(const String &uid, const String &name, long amount, long balanceAfter, const String &status)
  {
    if (!sd_card::isReady())
    {
      return false;
    }

    String line =
        rtc::getTimestamp() + "," +
        uid + "," +
        name + "," +
        String(amount) + "," +
        String(balanceAfter) + "," +
        status;

    return sd_card::appendLine(logPath, line);
  }
}
