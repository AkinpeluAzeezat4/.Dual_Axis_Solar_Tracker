#include <Arduino.h>
#include <WebServer.h>
#include <SD.h>
#include "local_server.h"
#include "attendance_manager/attendance_manager.h"
#include "sd_card/sd_card.h"
#include "WiFi/wifi_manager.h"

namespace local_server
{
  static WebServer server(80);
  static bool ready = false;

  static String htmlEscape(String value)
  {
    value.replace("&", "&amp;");
    value.replace("<", "&lt;");
    value.replace(">", "&gt;");
    value.replace("\"", "&quot;");
    value.replace("'", "&#39;");
    return value;
  }

  static String jsonEscape(String value)
  {
    value.replace("\\", "\\\\");
    value.replace("\"", "\\\"");
    value.replace("\n", "\\n");
    value.replace("\r", "\\r");
    value.replace("\t", "\\t");
    return value;
  }

  static String pageHead(const String &title)
  {
    String html;
    html += "<!doctype html><html><head>";
    html += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>" + htmlEscape(title) + "</title>";
    html += "<style>";
    html += "body{font-family:Arial,sans-serif;margin:18px;max-width:900px}";
    html += "input,select,button{padding:8px;margin:4px 0;width:100%;box-sizing:border-box}";
    html += "button{cursor:pointer}";
    html += "a{display:inline-block;margin:6px 8px 6px 0}";
    html += ".card{border:1px solid #ccc;border-radius:10px;padding:12px;margin:12px 0}";
    html += "pre{white-space:pre-wrap;background:#f5f5f5;padding:10px;border-radius:8px;overflow:auto}";
    html += "</style>";
    html += "</head><body>";
    html += "<h2>" + htmlEscape(title) + "</h2>";
    html += "<a href='/'>Home</a>";
    html += "<a href='/users'>Users</a>";
    html += "<a href='/attendance'>Attendance</a>";
    html += "<a href='/api/status'>Status API</a>";
    return html;
  }

  static String pageEnd()
  {
    return "</body></html>";
  }

  static void handleRoot()
  {
    String html = pageHead("Fingerprint RFID Attendance");

    html += "<div class='card'>";
    html += "<b>Device IP:</b> " + htmlEscape(wifi_manager::getIpString()) + "<br>";
    html += "<b>Mode:</b> " + htmlEscape(attendance_manager::getModeText()) + "<br>";
    html += "<b>Users:</b> " + String(attendance_manager::getUserCount()) + "<br>";
    html += "<b>Records:</b> " + String(attendance_manager::getAttendanceCount()) + "<br>";
    html += "<b>SD Card:</b> ";
    html += sd_card::isReady() ? "Ready" : "Not Ready";
    html += "<br>";
    html += "<b>Last event:</b> " + htmlEscape(attendance_manager::getLastMessage());
    html += "</div>";

    html += "<div class='card'><h3>Attendance Process</h3>";
    html += "<p>Tap RFID card first, then place the registered finger. Attendance is saved only when both match the same user.</p>";
    html += "</div>";

    html += "<div class='card'><h3>Set Attendance Mode</h3>";
    html += "<form action='/set-mode' method='get'>";
    html += "<select name='mode'>";
    html += "<option value='IN'>Clock In</option>";
    html += "<option value='OUT'>Clock Out</option>";
    html += "</select>";
    html += "<button type='submit'>Set Mode</button>";
    html += "</form></div>";

    html += "<div class='card'><h3>Register User</h3>";
    html += "<form action='/enroll' method='get'>";
    html += "<input name='id' placeholder='User ID e.g. STU001' required>";
    html += "<input name='name' placeholder='Full name' required>";
    html += "<input name='workspace' placeholder='Class / Department / Workspace' value='Main'>";
    html += "<button type='submit'>Start Registration</button>";
    html += "</form>";
    html += "<p>After starting registration, tap the RFID card and place the finger when the OLED asks.</p>";
    html += "</div>";

    html += "<div class='card'><h3>Downloads</h3>";
    html += "<a href='/download?file=users'>Download Users CSV</a>";
    html += "<a href='/download?file=attendance'>Download Attendance CSV</a>";
    html += "<a href='/download?file=config'>Download Config</a>";
    html += "</div>";

    html += pageEnd();
    server.send(200, "text/html", html);
  }

  static void handleSetMode()
  {
    String mode = server.arg("mode");
    mode.toUpperCase();

    if (mode == "OUT")
      attendance_manager::setMode(attendance_manager::MODE_OUT);
    else
      attendance_manager::setMode(attendance_manager::MODE_IN);

    server.sendHeader("Location", "/");
    server.send(303, "text/plain", "");
  }

  static void handleEnroll()
  {
    String userId = server.arg("id");
    String name = server.arg("name");
    String workspace = server.arg("workspace");

    bool ok = attendance_manager::startEnrollment(userId, name, workspace);

    String html = pageHead("Registration");
    html += "<div class='card'>";

    if (ok)
      html += "Registration started.<br>Tap RFID card, then place finger when requested.";
    else
      html += "Registration could not start: " + htmlEscape(attendance_manager::getLastMessage());

    html += "</div>";
    html += "<a href='/'>Back</a>";
    html += pageEnd();

    server.send(ok ? 200 : 409, "text/html", html);
  }

  static void handleUsers()
  {
    String html = pageHead("Users");

    if (!sd_card::isReady())
      html += "<div class='card'>SD card not ready.</div>";
    else
      html += "<pre>" + htmlEscape(sd_card::readFile("/users.csv", 18000)) + "</pre>";

    html += "<div class='card'><h3>Delete User</h3>";
    html += "<form action='/delete-user' method='get'>";
    html += "<input name='id' placeholder='User ID e.g. STU001' required>";
    html += "<button type='submit'>Delete User</button>";
    html += "</form>";
    html += "<p>This removes the user, RFID card link, and fingerprint template.</p>";
    html += "</div>";

    html += pageEnd();
    server.send(200, "text/html", html);
  }

  static void handleDeleteUser()
  {
    String userId = server.arg("id");
    userId.trim();

    bool ok = attendance_manager::deleteUser(userId);

    String html = pageHead("Delete User");
    html += "<div class='card'>";

    if (ok)
      html += "User deleted successfully.";
    else
      html += "Delete failed: " + htmlEscape(attendance_manager::getLastMessage());

    html += "</div>";
    html += "<a href='/users'>Back to Users</a>";
    html += pageEnd();

    server.send(ok ? 200 : 409, "text/html", html);
  }

  static void handleAttendance()
  {
    String html = pageHead("Attendance Log");

    if (!sd_card::isReady())
      html += "<div class='card'>SD card not ready.</div>";
    else
      html += "<pre>" + htmlEscape(sd_card::readFile("/attendance.csv", 24000)) + "</pre>";

    html += "<form action='/clear-attendance' method='get'>";
    html += "<input type='hidden' name='confirm' value='yes'>";
    html += "<button type='submit'>Clear Attendance Log</button>";
    html += "</form>";

    html += pageEnd();
    server.send(200, "text/html", html);
  }

  static void handleDownload()
  {
    String requested = server.arg("file");
    String path;
    String name;
    String type;

    if (requested == "users")
    {
      path = "/users.csv";
      name = "users.csv";
      type = "text/csv";
    }
    else if (requested == "attendance")
    {
      path = "/attendance.csv";
      name = "attendance.csv";
      type = "text/csv";
    }
    else if (requested == "config")
    {
      path = "/config.txt";
      name = "config.txt";
      type = "text/plain";
    }
    else
    {
      server.send(400, "text/plain", "Invalid file request");
      return;
    }

    if (!sd_card::isReady() || !SD.exists(path))
    {
      server.send(404, "text/plain", "File not found");
      return;
    }

    File file = SD.open(path, FILE_READ);

    if (!file)
    {
      server.send(500, "text/plain", "Could not open file");
      return;
    }

    server.sendHeader("Content-Disposition", String("attachment; filename=") + name);
    server.streamFile(file, type);
    file.close();
  }

  static void handleClearAttendance()
  {
    if (server.arg("confirm") == "yes")
      sd_card::clearAttendance();

    server.sendHeader("Location", "/attendance");
    server.send(303, "text/plain", "");
  }

  static void handleStatusApi()
  {
    String json = "{";
    json += "\"ip\":\"" + jsonEscape(wifi_manager::getIpString()) + "\",";
    json += "\"mode\":\"" + jsonEscape(attendance_manager::getModeText()) + "\",";
    json += "\"users\":" + String(attendance_manager::getUserCount()) + ",";
    json += "\"attendance_records\":" + String(attendance_manager::getAttendanceCount()) + ",";
    json += "\"sd_ready\":";
    json += sd_card::isReady() ? "true" : "false";
    json += ",";
    json += "\"last_message\":\"" + jsonEscape(attendance_manager::getLastMessage()) + "\"";
    json += "}";

    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", json);
  }

  static void handleNotFound()
  {
    server.send(404, "text/plain", "Not found");
  }

  void begin()
  {
    server.on("/", handleRoot);
    server.on("/set-mode", handleSetMode);
    server.on("/enroll", handleEnroll);
    server.on("/users", handleUsers);
    server.on("/delete-user", handleDeleteUser);
    server.on("/attendance", handleAttendance);
    server.on("/download", handleDownload);
    server.on("/clear-attendance", handleClearAttendance);
    server.on("/api/status", handleStatusApi);
    server.onNotFound(handleNotFound);

    server.begin();
    ready = true;
  }

  void update()
  {
    if (ready)
      server.handleClient();
  }

  bool isReady()
  {
    return ready;
  }
}
