#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "WiFi.h"
#include "p_data/p_data.h"
#include "payment/payment.h"

namespace
{
  WebServer server(80);
  bool ready = false;

  const char *apSsid = "NFC_PAYMENT_TERMINAL";
  const char *apPassword = "12345678";

  String htmlEscape(String text)
  {
    text.replace("&", "&amp;");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    text.replace("\"", "&quot;");
    text.replace("'", "&#39;");
    return text;
  }

  String pageHeader()
  {
    String html;
    html += F("<!doctype html><html><head><meta charset='utf-8'>");
    html += F("<meta name='viewport' content='width=device-width,initial-scale=1'>");
    html += F("<meta http-equiv='refresh' content='5'>");
    html += F("<title>NFC Payment Terminal</title>");
    html += F("<style>"
              "body{font-family:Arial,sans-serif;background:#111;color:#eee;padding:16px;}"
              ".card{background:#1b1b1b;border-radius:12px;padding:16px;margin-bottom:14px;}"
              "input,button{padding:10px;border-radius:8px;border:1px solid #444;background:#222;color:#eee;width:100%;box-sizing:border-box;margin-top:8px;}"
              "button{background:#2e7d32;border:none;font-weight:700;cursor:pointer;}"
              "a{color:#90caf9;text-decoration:none;}"
              "small{color:#aaa;}"
              "</style></head><body>");
    html += F("<h2>NFC Card Payment Terminal</h2>");
    return html;
  }

  String pageFooter()
  {
    return F("</body></html>");
  }

  void redirectRoot()
  {
    server.sendHeader("Location", "/", true);
    server.send(303);
  }

  void handleRoot()
  {
    String pending = p_data::getPendingUID();
    String html = pageHeader();

    html += F("<div class='card'>");
    html += F("<b>Access Point IP:</b> ");
    html += wifi_service::getIP();
    html += F("<br><b>Selected Amount:</b> ");
    html += payment::getSelectedAmountText();
    html += F("<br><b>Stored Cards:</b> ");
    html += String(p_data::countCards());
    html += F("<br><b>Pending UID:</b> ");
    html += (pending.length() ? htmlEscape(pending) : String("None"));
    html += F("</div>");

    html += F("<div class='card'><h3>Register / Update Card</h3>");
    html += F("<form method='POST' action='/register'>");
    html += F("<small>Leave UID empty to use the latest unregistered scanned card.</small>");
    html += F("<input name='uid' placeholder='UID (optional)'>");
    html += F("<input name='name' placeholder='Card holder name' required>");
    html += F("<input name='balance' type='number' min='0' value='0' placeholder='Opening balance'>");
    html += F("<button type='submit'>Save Card</button></form></div>");

    html += F("<div class='card'><h3>Top Up Balance</h3>");
    html += F("<form method='POST' action='/topup'>");
    html += F("<input name='uid' placeholder='UID' required>");
    html += F("<input name='amount' type='number' min='1' value='100' required>");
    html += F("<button type='submit'>Top Up</button></form></div>");

    html += F("<div class='card'><h3>Database</h3>");
    html += F("<a href='/cards'>Open cards.csv</a>");
    html += F("</div>");

    html += pageFooter();
    server.send(200, "text/html", html);
  }

  void handleRegister()
  {
    String uid = server.arg("uid");
    uid.trim();

    if (uid.length() == 0)
    {
      uid = p_data::getPendingUID();
    }

    String name = server.arg("name");
    name.trim();

    long openingBalance = server.arg("balance").toInt();

    if (uid.length() == 0 || name.length() == 0)
    {
      server.send(400, "text/plain", "UID or name missing");
      return;
    }

    if (!p_data::registerCard(uid, name, openingBalance))
    {
      server.send(400, "text/plain", "Register failed");
      return;
    }

    redirectRoot();
  }

  void handleTopup()
  {
    String uid = server.arg("uid");
    uid.trim();

    long amount = server.arg("amount").toInt();
    long newBalance = 0;

    if (uid.length() == 0 || amount <= 0)
    {
      server.send(400, "text/plain", "Invalid top up request");
      return;
    }

    if (!p_data::topUpCard(uid, amount, newBalance))
    {
      server.send(400, "text/plain", "Top up failed");
      return;
    }

    redirectRoot();
  }

  void handleCards()
  {
    server.send(200, "text/plain", p_data::exportCardsCsv());
  }

  void handleNotFound()
  {
    redirectRoot();
  }
}

namespace wifi_service
{
  void begin()
  {
    ::WiFi.mode(WIFI_AP);
    ready = ::WiFi.softAP(apSsid, apPassword);

    if (!ready)
    {
      return;
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/register", HTTP_POST, handleRegister);
    server.on("/topup", HTTP_POST, handleTopup);
    server.on("/cards", HTTP_GET, handleCards);
    server.onNotFound(handleNotFound);
    server.begin();
  }

  void update()
  {
    if (!ready)
      return;

    server.handleClient();
  }

  bool isConnected()
  {
    return ::WiFi.softAPgetStationNum() > 0;
  }

  bool isReady()
  {
    return ready;
  }

  String getIP()
  {
    if (!ready)
      return String("0.0.0.0");

    return ::WiFi.softAPIP().toString();
  }
}
