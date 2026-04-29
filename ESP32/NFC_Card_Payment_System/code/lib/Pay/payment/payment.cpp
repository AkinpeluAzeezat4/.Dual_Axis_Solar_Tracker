#include <Arduino.h>
#include "payment.h"
#include "p_data/p_data.h"
#include "transaction_log/transaction_log.h"

namespace
{
  const long presets[] = {100, 200, 500, 1000, 2000, 5000, 10000};
  const uint8_t presetCount = sizeof(presets) / sizeof(presets[0]);
  uint8_t selectedIndex = 0;
}

namespace payment
{
  void begin()
  {
    selectedIndex = 0;
  }

  void update()
  {
  }

  void nextAmount()
  {
    if (selectedIndex + 1 < presetCount)
    {
      ++selectedIndex;
    }
  }

  void previousAmount()
  {
    if (selectedIndex > 0)
    {
      --selectedIndex;
    }
  }

  long getSelectedAmount()
  {
    return presets[selectedIndex];
  }

  String getSelectedAmountText()
  {
    return "N" + String(getSelectedAmount());
  }

  Result processCard(const String &uid)
  {
    Result result;
    result.amount = getSelectedAmount();

    p_data::CardRecord existing;
    if (!p_data::findCard(uid, existing))
    {
      result.success = false;
      result.reason = PAY_UNKNOWN_CARD;
      result.message = "Card not registered";
      return result;
    }

    p_data::CardRecord updated;
    if (!p_data::deductCard(uid, result.amount, updated))
    {
      result.success = false;
      result.reason = PAY_INSUFFICIENT_FUNDS;
      result.holderName = existing.name;
      result.balanceAfter = existing.balance;
      result.message = "Insufficient funds";
      transaction_log::log(uid, existing.name, result.amount, existing.balance, "FAILED");
      return result;
    }

    result.success = true;
    result.reason = PAY_OK;
    result.holderName = updated.name;
    result.balanceAfter = updated.balance;
    result.message = "Payment successful";
    transaction_log::log(uid, updated.name, result.amount, updated.balance, "SUCCESS");
    return result;
  }
}
