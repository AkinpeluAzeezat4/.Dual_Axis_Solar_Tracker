#include <Arduino.h>
#include "transaction_log.h"
#include "sd_card/sd_card.h"
#include "rtc/rtc.h"

namespace
{
  const char *logPath = "/logs/transactions.csv";

  String csvSafe(const String &input)
  {
    String text = input;
    text.replace(",", " ");
    text.replace("\n", " ");
    text.replace("\r", " ");
    text.trim();
    return text;
  }
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
        csvSafe(rtc::getTimestamp()) + "," +
        csvSafe(uid) + "," +
        csvSafe(name) + "," +
        String(amount) + "," +
        String(balanceAfter) + "," +
        csvSafe(status);

    return sd_card::appendLine(logPath, line);
  }
}
