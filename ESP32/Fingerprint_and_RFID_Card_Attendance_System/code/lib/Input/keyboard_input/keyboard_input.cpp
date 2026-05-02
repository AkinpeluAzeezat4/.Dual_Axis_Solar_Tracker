#include <Arduino.h>
#include "keyboard_input.h"
#include "attendance_manager/attendance_manager.h"

namespace keyboard_input
{
  static String buffer;
  static String lastCommand = "None";

  static String partAt(const String &text, uint8_t index)
  {
    int start = 0;
    uint8_t current = 0;

    for (uint16_t i = 0; i <= text.length(); i++)
    {
      if (i == text.length() || text[i] == ',')
      {
        if (current == index)
        {
          String part = text.substring(start, i);
          part.trim();
          return part;
        }

        start = i + 1;
        current++;
      }
    }

    return "";
  }

  static void printHelp()
  {
    Serial.println();
    Serial.println("Prototype commands:");
    Serial.println("IN");
    Serial.println("OUT");
    Serial.println("STATUS");
    Serial.println("ENROLL,STU001,John Doe,Main");
    Serial.println("DELETE,STU001");
    Serial.println();
  }

  static void processLine(String line)
  {
    line.trim();

    if (line.length() == 0)
      return;

    lastCommand = line;

    String upper = line;
    upper.toUpperCase();

    if (upper == "HELP")
    {
      printHelp();
      return;
    }

    if (upper == "IN")
    {
      attendance_manager::setMode(attendance_manager::MODE_IN);
      Serial.println("Mode set to IN");
      return;
    }

    if (upper == "OUT")
    {
      attendance_manager::setMode(attendance_manager::MODE_OUT);
      Serial.println("Mode set to OUT");
      return;
    }

    if (upper == "STATUS")
    {
      Serial.println();
      Serial.println("Attendance System Status");
      Serial.println("------------------------");
      Serial.println("Mode: " + attendance_manager::getModeText());
      Serial.println("Users: " + String(attendance_manager::getUserCount()));
      Serial.println("Records: " + String(attendance_manager::getAttendanceCount()));
      Serial.println("Enrollment busy: " + String(attendance_manager::isEnrollmentBusy() ? "YES" : "NO"));
      Serial.println("Last: " + attendance_manager::getLastMessage());
      Serial.println();
      return;
    }

    if (upper.startsWith("ENROLL,"))
    {
      String userId = partAt(line, 1);
      String name = partAt(line, 2);
      String workspace = partAt(line, 3);

      if (workspace.length() == 0)
        workspace = "Main";

      if (userId.length() == 0 || name.length() == 0)
      {
        Serial.println("Enrollment failed: User ID and name are required");
        Serial.println("Format: ENROLL,STU001,John Doe,Main");
        return;
      }

      bool ok = attendance_manager::startEnrollment(userId, name, workspace);

      if (ok)
      {
        Serial.println("Enrollment started");
        Serial.println("Tap RFID card first, then place finger when OLED asks");
      }
      else
      {
        Serial.println("Enrollment failed: " + attendance_manager::getLastMessage());
      }

      return;
    }

    if (upper.startsWith("DELETE,"))
    {
      String userId = partAt(line, 1);

      if (userId.length() == 0)
      {
        Serial.println("Delete failed: User ID is required");
        Serial.println("Format: DELETE,STU001");
        return;
      }

      bool ok = attendance_manager::deleteUser(userId);

      if (ok)
      {
        Serial.println("User deleted");
        Serial.println("RFID link removed and fingerprint template deleted");
      }
      else
      {
        Serial.println("Delete failed: " + attendance_manager::getLastMessage());
      }

      return;
    }

    Serial.println("Unknown command");
    printHelp();
  }

  void begin()
  {
    buffer.reserve(128);
    lastCommand = "None";

    Serial.println("Keyboard input module ready");
    printHelp();
  }

  void update()
  {
    while (Serial.available())
    {
      char c = (char)Serial.read();

      if (c == '\r')
        continue;

      if (c == '\n')
      {
        processLine(buffer);
        buffer = "";
        continue;
      }

      if (buffer.length() < 120)
        buffer += c;
    }
  }

  String getLastCommand()
  {
    return lastCommand;
  }
}
