#ifndef P_DATA_H
#define P_DATA_H

#include <Arduino.h>

namespace p_data
{
  struct CardRecord
  {
    String uid = "";
    String name = "";
    long balance = 0;
    bool valid = false;
  };

  void begin();
  bool isReady();

  bool hasPendingUID();
  void setPendingUID(const String &uid);
  String getPendingUID();
  void clearPendingUID();

  bool findCard(const String &uid, CardRecord &record);
  bool registerCard(const String &uid, const String &name, long openingBalance);
  bool topUpCard(const String &uid, long amount, long &newBalance);
  bool deductCard(const String &uid, long amount, CardRecord &updatedRecord);

  size_t countCards();
  String exportCardsCsv();
}

#endif
