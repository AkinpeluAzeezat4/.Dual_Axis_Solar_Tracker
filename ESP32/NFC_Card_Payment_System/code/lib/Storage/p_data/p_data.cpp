#include <Arduino.h>
#include <SD.h>
#include "p_data.h"
#include "sd_card/sd_card.h"

namespace
{
  const char *cardsPath = "/db/cards.csv";
  const char *tempPath = "/db/cards.tmp";

  bool ready = false;
  String pendingUID;

  String emptyFieldSafe(const String &input)
  {
    String text = input;
    text.replace(",", " ");
    text.trim();
    return text;
  }

  bool parseLine(const String &line, p_data::CardRecord &record)
  {
    if (line.length() == 0 || line.startsWith("uid,"))
    {
      return false;
    }

    int first = line.indexOf(',');
    int second = line.indexOf(',', first + 1);

    if (first < 0 || second < 0)
    {
      return false;
    }

    record.uid = line.substring(0, first);
    record.name = line.substring(first + 1, second);
    record.balance = line.substring(second + 1).toInt();
    record.valid = true;
    return true;
  }

  String formatLine(const p_data::CardRecord &record)
  {
    return emptyFieldSafe(record.uid) + "," +
           emptyFieldSafe(record.name) + "," +
           String(record.balance);
  }

  bool ensureHeader()
  {
    if (!sd_card::isReady())
    {
      return false;
    }

    if (!sd_card::exists(cardsPath))
    {
      return sd_card::appendLine(cardsPath, "uid,name,balance");
    }

    return true;
  }

  bool replaceDatabaseWithTemp()
  {
    SD.remove(cardsPath);
    return SD.rename(tempPath, cardsPath);
  }
}

namespace p_data
{
  void begin()
  {
    ready = ensureHeader();
  }

  bool isReady()
  {
    return ready;
  }

  bool hasPendingUID()
  {
    return pendingUID.length() > 0;
  }

  void setPendingUID(const String &uid)
  {
    pendingUID = uid;
  }

  String getPendingUID()
  {
    return pendingUID;
  }

  void clearPendingUID()
  {
    pendingUID = "";
  }

  bool findCard(const String &uid, CardRecord &record)
  {
    record = CardRecord();

    if (!ready)
    {
      return false;
    }

    File file = SD.open(cardsPath, FILE_READ);
    if (!file)
    {
      return false;
    }

    while (file.available())
    {
      String line = file.readStringUntil('\n');
      line.trim();

      CardRecord current;
      if (!parseLine(line, current))
      {
        continue;
      }

      if (current.uid == uid)
      {
        record = current;
        file.close();
        return true;
      }
    }

    file.close();
    return false;
  }

  bool registerCard(const String &uid, const String &name, long openingBalance)
  {
    if (!ready || uid.length() == 0 || name.length() == 0)
    {
      return false;
    }

    SD.remove(tempPath);

    File in = SD.open(cardsPath, FILE_READ);
    File out = SD.open(tempPath, FILE_WRITE);

    if (!in || !out)
    {
      if (in)
        in.close();
      if (out)
        out.close();
      SD.remove(tempPath);
      return false;
    }

    out.println("uid,name,balance");

    bool updated = false;

    while (in.available())
    {
      String line = in.readStringUntil('\n');
      line.trim();

      CardRecord record;
      if (!parseLine(line, record))
      {
        continue;
      }

      if (record.uid == uid)
      {
        record.name = emptyFieldSafe(name);
        record.balance = openingBalance;
        updated = true;
      }

      out.println(formatLine(record));
    }

    if (!updated)
    {
      CardRecord record;
      record.uid = emptyFieldSafe(uid);
      record.name = emptyFieldSafe(name);
      record.balance = openingBalance;
      record.valid = true;
      out.println(formatLine(record));
    }

    in.close();
    out.close();

    if (!replaceDatabaseWithTemp())
    {
      SD.remove(tempPath);
      return false;
    }

    if (pendingUID == uid)
    {
      clearPendingUID();
    }

    return true;
  }

  bool topUpCard(const String &uid, long amount, long &newBalance)
  {
    newBalance = 0;

    if (!ready || uid.length() == 0 || amount <= 0)
    {
      return false;
    }

    SD.remove(tempPath);

    File in = SD.open(cardsPath, FILE_READ);
    File out = SD.open(tempPath, FILE_WRITE);

    if (!in || !out)
    {
      if (in)
        in.close();
      if (out)
        out.close();
      SD.remove(tempPath);
      return false;
    }

    out.println("uid,name,balance");

    bool found = false;

    while (in.available())
    {
      String line = in.readStringUntil('\n');
      line.trim();

      CardRecord record;
      if (!parseLine(line, record))
      {
        continue;
      }

      if (record.uid == uid)
      {
        record.balance += amount;
        newBalance = record.balance;
        found = true;
      }

      out.println(formatLine(record));
    }

    in.close();
    out.close();

    if (!found)
    {
      SD.remove(tempPath);
      return false;
    }

    if (!replaceDatabaseWithTemp())
    {
      SD.remove(tempPath);
      return false;
    }

    return true;
  }

  bool deductCard(const String &uid, long amount, CardRecord &updatedRecord)
  {
    updatedRecord = CardRecord();

    if (!ready || uid.length() == 0 || amount <= 0)
    {
      return false;
    }

    SD.remove(tempPath);

    File in = SD.open(cardsPath, FILE_READ);
    File out = SD.open(tempPath, FILE_WRITE);

    if (!in || !out)
    {
      if (in)
        in.close();
      if (out)
        out.close();
      SD.remove(tempPath);
      return false;
    }

    out.println("uid,name,balance");

    bool found = false;
    bool enough = false;

    while (in.available())
    {
      String line = in.readStringUntil('\n');
      line.trim();

      CardRecord record;
      if (!parseLine(line, record))
      {
        continue;
      }

      if (record.uid == uid)
      {
        found = true;

        if (record.balance >= amount)
        {
          record.balance -= amount;
          enough = true;
          updatedRecord = record;
          updatedRecord.valid = true;
        }
      }

      out.println(formatLine(record));
    }

    in.close();
    out.close();

    if (!found || !enough)
    {
      SD.remove(tempPath);
      return false;
    }

    if (!replaceDatabaseWithTemp())
    {
      SD.remove(tempPath);
      return false;
    }

    return true;
  }

  size_t countCards()
  {
    if (!ready)
    {
      return 0;
    }

    size_t count = 0;
    File file = SD.open(cardsPath, FILE_READ);
    if (!file)
    {
      return 0;
    }

    while (file.available())
    {
      String line = file.readStringUntil('\n');
      line.trim();

      CardRecord record;
      if (parseLine(line, record))
      {
        ++count;
      }
    }

    file.close();
    return count;
  }

  String exportCardsCsv()
  {
    if (!ready)
    {
      return "SD not ready";
    }

    return sd_card::readText(cardsPath);
  }
}
