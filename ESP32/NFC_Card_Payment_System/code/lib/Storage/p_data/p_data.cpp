#include <Arduino.h>
#include "p_data.h"
#include "sd_card/sd_card.h"

namespace
{
  const char *cardsPath = "/db/cards.csv";

  bool ready = false;
  String pendingUID;

  String emptyFieldSafe(const String &input)
  {
    String text = input;
    text.replace(",", " ");
    text.replace("\n", " ");
    text.replace("\r", " ");
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

  bool readNextLine(const String &content, int &index, String &line)
  {
    if (index >= content.length())
    {
      return false;
    }

    int next = content.indexOf('\n', index);

    if (next < 0)
    {
      line = content.substring(index);
      index = content.length();
    }
    else
    {
      line = content.substring(index, next);
      index = next + 1;
    }

    line.trim();
    return true;
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

    if (!ready || uid.length() == 0)
    {
      return false;
    }

    String content = sd_card::readText(cardsPath);
    int index = 0;
    String line;

    while (readNextLine(content, index, line))
    {
      CardRecord current;

      if (!parseLine(line, current))
      {
        continue;
      }

      if (current.uid == uid)
      {
        record = current;
        return true;
      }
    }

    return false;
  }

  bool registerCard(const String &uid, const String &name, long openingBalance)
  {
    if (!ready || uid.length() == 0 || name.length() == 0)
    {
      return false;
    }

    String content = sd_card::readText(cardsPath);
    String newContent = "uid,name,balance\n";

    int index = 0;
    String line;
    bool updated = false;

    while (readNextLine(content, index, line))
    {
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

      newContent += formatLine(record) + "\n";
    }

    if (!updated)
    {
      CardRecord record;
      record.uid = emptyFieldSafe(uid);
      record.name = emptyFieldSafe(name);
      record.balance = openingBalance;
      record.valid = true;
      newContent += formatLine(record) + "\n";
    }

    if (!sd_card::writeText(cardsPath, newContent))
    {
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

    String content = sd_card::readText(cardsPath);
    String newContent = "uid,name,balance\n";

    int index = 0;
    String line;
    bool found = false;

    while (readNextLine(content, index, line))
    {
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

      newContent += formatLine(record) + "\n";
    }

    if (!found)
    {
      return false;
    }

    return sd_card::writeText(cardsPath, newContent);
  }

  bool deductCard(const String &uid, long amount, CardRecord &updatedRecord)
  {
    updatedRecord = CardRecord();

    if (!ready || uid.length() == 0 || amount <= 0)
    {
      return false;
    }

    String content = sd_card::readText(cardsPath);
    String newContent = "uid,name,balance\n";

    int index = 0;
    String line;
    bool found = false;
    bool enough = false;

    while (readNextLine(content, index, line))
    {
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

      newContent += formatLine(record) + "\n";
    }

    if (!found || !enough)
    {
      return false;
    }

    return sd_card::writeText(cardsPath, newContent);
  }

  bool deleteCard(const String &uid)
  {
    if (!ready || uid.length() == 0)
    {
      return false;
    }

    String content = sd_card::readText(cardsPath);
    String newContent = "uid,name,balance\n";

    int index = 0;
    String line;
    bool deleted = false;

    while (readNextLine(content, index, line))
    {
      CardRecord record;

      if (!parseLine(line, record))
      {
        continue;
      }

      if (record.uid == uid)
      {
        deleted = true;
        continue;
      }

      newContent += formatLine(record) + "\n";
    }

    if (!deleted)
    {
      return false;
    }

    if (pendingUID == uid)
    {
      clearPendingUID();
    }

    return sd_card::writeText(cardsPath, newContent);
  }

  bool clearAllCards()
  {
    if (!ready)
    {
      return false;
    }

    pendingUID = "";
    return sd_card::writeText(cardsPath, "uid,name,balance\n");
  }

  size_t countCards()
  {
    if (!ready)
    {
      return 0;
    }

    size_t count = 0;
    String content = sd_card::readText(cardsPath);

    int index = 0;
    String line;

    while (readNextLine(content, index, line))
    {
      CardRecord record;

      if (parseLine(line, record))
      {
        count++;
      }
    }

    return count;
  }

  String exportCardsCsv()
  {
    if (!ready)
    {
      return "Storage not ready";
    }

    return sd_card::readText(cardsPath);
  }
}
