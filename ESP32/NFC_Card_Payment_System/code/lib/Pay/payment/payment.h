#ifndef PAYMENT_H
#define PAYMENT_H

#include <Arduino.h>

namespace payment
{
  enum ResultReason
  {
    PAY_OK = 0,
    PAY_UNKNOWN_CARD,
    PAY_INSUFFICIENT_FUNDS
  };

  struct Result
  {
    bool success = false;
    ResultReason reason = PAY_UNKNOWN_CARD;
    String holderName = "";
    long amount = 0;
    long balanceAfter = 0;
    String message = "";
  };

  void begin();
  void update();

  void nextAmount();
  void previousAmount();

  long getSelectedAmount();
  String getSelectedAmountText();

  Result processCard(const String &uid);
}

#endif
