#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "WiFi.h"
#include "p_data/p_data.h"
#include "payment/payment.h"
#include "sd_card/sd_card.h"

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
    html += F("<title>NFC Payment Terminal</title>");

    html += F("<style>");
    html += F(
        ":root{"
        "--bg:#0f172a;"
        "--bg2:#111827;"
        "--card:#1e293b;"
        "--text:#e5e7eb;"
        "--muted:#94a3b8;"
        "--line:#334155;"
        "--accent:#38bdf8;"
        "--accent2:#22c55e;"
        "--danger:#ef4444;"
        "--input:#0f172a;"
        "--shadow:0 18px 50px rgba(0,0,0,.28);"
        "}"
        "body.light{"
        "--bg:#eef4ff;"
        "--bg2:#f8fafc;"
        "--card:#ffffff;"
        "--text:#0f172a;"
        "--muted:#64748b;"
        "--line:#dbe3ef;"
        "--accent:#0284c7;"
        "--accent2:#16a34a;"
        "--danger:#dc2626;"
        "--input:#f8fafc;"
        "--shadow:0 18px 40px rgba(15,23,42,.10);"
        "}"
        "*{box-sizing:border-box;}"
        "body{"
        "margin:0;"
        "font-family:Arial,sans-serif;"
        "background:linear-gradient(135deg,var(--bg),var(--bg2));"
        "color:var(--text);"
        "min-height:100vh;"
        "padding:16px;"
        "transition:background .25s,color .25s;"
        "}"
        ".wrap{"
        "width:100%;"
        "max-width:760px;"
        "margin:0 auto;"
        "}"
        ".topbar{"
        "display:flex;"
        "align-items:center;"
        "justify-content:space-between;"
        "gap:12px;"
        "margin-bottom:16px;"
        "}"
        ".brand{"
        "display:flex;"
        "align-items:center;"
        "gap:12px;"
        "}"
        ".logo{"
        "width:44px;"
        "height:44px;"
        "border-radius:14px;"
        "background:linear-gradient(135deg,var(--accent),var(--accent2));"
        "display:flex;"
        "align-items:center;"
        "justify-content:center;"
        "font-weight:800;"
        "color:white;"
        "box-shadow:var(--shadow);"
        "}"
        "h1{font-size:22px;margin:0;line-height:1.2;}"
        ".subtitle{font-size:13px;color:var(--muted);margin-top:3px;}"
        ".themeBtn{"
        "width:44px;"
        "height:44px;"
        "border-radius:14px;"
        "border:1px solid var(--line);"
        "background:var(--card);"
        "color:var(--text);"
        "font-size:20px;"
        "cursor:pointer;"
        "box-shadow:var(--shadow);"
        "}"
        ".grid{"
        "display:grid;"
        "grid-template-columns:repeat(2,minmax(0,1fr));"
        "gap:14px;"
        "}"
        ".card{"
        "background:var(--card);"
        "border:1px solid var(--line);"
        "border-radius:18px;"
        "padding:16px;"
        "margin-bottom:14px;"
        "box-shadow:var(--shadow);"
        "}"
        ".span2{grid-column:span 2;}"
        ".statusGrid{"
        "display:grid;"
        "grid-template-columns:repeat(2,minmax(0,1fr));"
        "gap:10px;"
        "}"
        ".stat{"
        "background:rgba(148,163,184,.08);"
        "border:1px solid var(--line);"
        "border-radius:14px;"
        "padding:12px;"
        "}"
        ".label{font-size:12px;color:var(--muted);margin-bottom:5px;}"
        ".value{font-size:15px;font-weight:700;word-break:break-word;}"
        ".pill{"
        "display:inline-flex;"
        "align-items:center;"
        "gap:6px;"
        "font-size:12px;"
        "padding:6px 10px;"
        "border-radius:999px;"
        "background:rgba(34,197,94,.12);"
        "color:var(--accent2);"
        "border:1px solid rgba(34,197,94,.28);"
        "margin-top:10px;"
        "}"
        "h3{margin:0 0 10px;font-size:17px;}"
        "small{display:block;color:var(--muted);line-height:1.4;margin-bottom:10px;}"
        "input{"
        "padding:12px;"
        "border-radius:12px;"
        "border:1px solid var(--line);"
        "background:var(--input);"
        "color:var(--text);"
        "width:100%;"
        "margin-top:9px;"
        "outline:none;"
        "font-size:15px;"
        "}"
        "input:focus{border-color:var(--accent);box-shadow:0 0 0 3px rgba(56,189,248,.14);}"
        "button.submit{"
        "width:100%;"
        "padding:12px;"
        "border-radius:12px;"
        "border:none;"
        "background:linear-gradient(135deg,var(--accent),var(--accent2));"
        "color:white;"
        "font-weight:800;"
        "cursor:pointer;"
        "margin-top:12px;"
        "font-size:15px;"
        "}"
        "button.danger{background:linear-gradient(135deg,#ef4444,#f97316);}"
        "a{color:var(--accent);text-decoration:none;font-weight:700;}"
        ".hint{color:var(--muted);font-size:13px;line-height:1.5;}"
        ".footer{"
        "text-align:center;"
        "color:var(--muted);"
        "font-size:12px;"
        "padding:10px 0 18px;"
        "}"
        "@media(max-width:680px){"
        "body{padding:12px;}"
        ".grid{grid-template-columns:1fr;}"
        ".span2{grid-column:span 1;}"
        ".statusGrid{grid-template-columns:1fr;}"
        "h1{font-size:19px;}"
        ".logo,.themeBtn{width:40px;height:40px;border-radius:12px;}"
        "}"
        "@media(max-width:380px){"
        "body{padding:10px;}"
        ".card{padding:14px;border-radius:16px;}"
        ".topbar{align-items:flex-start;}"
        "h1{font-size:17px;}"
        ".subtitle{font-size:12px;}"
        "}");

    html += F("</style>");

    html += F("<script>");
    html += F(
        "function applyTheme(t){"
        "document.body.className=t;"
        "localStorage.setItem('theme',t);"
        "document.getElementById('themeIcon').textContent=t==='light'?'🌙':'☀️';"
        "}"
        "function toggleTheme(){"
        "var t=document.body.className==='light'?'':'light';"
        "applyTheme(t);"
        "}"
        "window.addEventListener('DOMContentLoaded',function(){"
        "applyTheme(localStorage.getItem('theme')||'');"
        "});");
    html += F("</script>");

    html += F("</head><body>");
    html += F("<div class='wrap'>");

    html += F("<div class='topbar'>");
    html += F("<div class='brand'>");
    html += F("<div class='logo'>N</div>");
    html += F("<div>");
    html += F("<h1>NFC Payment Terminal</h1>");
    html += F("<div class='subtitle'>Offline card payment prototype</div>");
    html += F("</div>");
    html += F("</div>");
    html += F("<button class='themeBtn' onclick='toggleTheme()' title='Toggle theme'><span id='themeIcon'>☀️</span></button>");
    html += F("</div>");

    return html;
  }

  String pageFooter()
  {
    String html;
    html += F("<div class='footer'>ESP32-S3 Local Server • SD/Internal Storage Backend</div>");
    html += F("</div></body></html>");
    return html;
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

    html += F("<div class='card span2'>");
    html += F("<div class='statusGrid'>");

    html += F("<div class='stat'><div class='label'>Access Point IP</div><div class='value'>");
    html += wifi_service::getIP();
    html += F("</div></div>");

    html += F("<div class='stat'><div class='label'>Selected Amount</div><div class='value'>");
    html += payment::getSelectedAmountText();
    html += F("</div></div>");

    html += F("<div class='stat'><div class='label'>Stored Cards</div><div class='value'>");
    html += String(p_data::countCards());
    html += F("</div></div>");

    html += F("<div class='stat'><div class='label'>Storage Backend</div><div class='value'>");
    html += sd_card::getBackendName();
    html += F("</div></div>");

    html += F("<div class='stat span2'><div class='label'>Pending UID</div><div class='value'>");
    html += (pending.length() ? htmlEscape(pending) : String("None"));
    html += F("</div></div>");

    html += F("</div>");
    html += F("<div class='pill'>● Terminal Ready</div>");
    html += F("</div>");

    html += F("<div class='grid'>");

    html += F("<div class='card'>");
    html += F("<h3>Register / Update Card</h3>");
    html += F("<form method='POST' action='/register'>");
    html += F("<small>Leave UID empty to use the latest unregistered scanned card.</small>");

    if (pending.length())
    {
      html += F("<div class='hint'><b>Pending UID:</b> ");
      html += htmlEscape(pending);
      html += F("</div>");
    }

    html += F("<input name='uid' placeholder='UID (optional)'>");
    html += F("<input name='name' placeholder='Card holder name' required>");
    html += F("<input name='balance' type='number' min='0' value='0' placeholder='Opening balance'>");
    html += F("<button class='submit' type='submit'>Save Card</button>");
    html += F("</form>");
    html += F("</div>");

    html += F("<div class='card'>");
    html += F("<h3>Top Up Balance</h3>");
    html += F("<form method='POST' action='/topup'>");
    html += F("<small>Add value to an existing registered card.</small>");
    html += F("<input name='uid' placeholder='UID' required>");
    html += F("<input name='amount' type='number' min='1' value='100' required>");
    html += F("<button class='submit' type='submit'>Top Up</button>");
    html += F("</form>");
    html += F("</div>");

    html += F("<div class='card span2'>");
    html += F("<div class='card'>");
    html += F("<h3>Delete Card</h3>");
    html += F("<form method='POST' action='/delete_card'>");
    html += F("<small>Remove one card record from the active storage backend.</small>");
    html += F("<input name='uid' placeholder='UID to delete' required>");
    html += F("<button class='submit danger' type='submit'>Delete Card</button>");
    html += F("</form>");
    html += F("</div>");

    html += F("<div class='card'>");
    html += F("<h3>Clear All Cards</h3>");
    html += F("<form method='POST' action='/clear_all'>");
    html += F("<small>This resets the card database. Type CLEAR to confirm.</small>");
    html += F("<input name='confirm' placeholder='Type CLEAR to confirm' required>");
    html += F("<button class='submit danger' type='submit'>Clear All Cards</button>");
    html += F("</form>");
    html += F("</div>");
    html += F("<h3>Database</h3>");
    html += F("<div class='hint'>View the card database currently stored in the active backend.</div>");
    html += F("<br><a href='/cards'>Open cards.csv</a>");
    html += F("</div>");

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

    if (uid.length() == 0)
    {
      server.send(400, "text/plain", "No UID found. Tap an unregistered card first or enter UID manually.");
      return;
    }

    if (name.length() == 0)
    {
      server.send(400, "text/plain", "Name missing. Enter card holder name.");
      return;
    }

    if (!p_data::registerCard(uid, name, openingBalance))
    {
      server.send(400, "text/plain", "Register failed");
      return;
    }

    redirectRoot();
  }
  void handleDeleteCard()
  {
    String uid = server.arg("uid");
    uid.trim();

    if (uid.length() == 0)
    {
      server.send(400, "text/plain", "UID missing");
      return;
    }

    if (!p_data::deleteCard(uid))
    {
      server.send(400, "text/plain", "Delete failed. UID may not exist.");
      return;
    }

    redirectRoot();
  }

  void handleClearAllCards()
  {
    String confirm = server.arg("confirm");
    confirm.trim();

    if (confirm != "CLEAR")
    {
      server.send(400, "text/plain", "Type CLEAR to confirm clearing all cards.");
      return;
    }

    if (!p_data::clearAllCards())
    {
      server.send(400, "text/plain", "Clear all failed.");
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
    server.on("/delete_card", HTTP_POST, handleDeleteCard);
    server.on("/clear_all", HTTP_POST, handleClearAllCards);
    server.on("/cards", HTTP_GET, handleCards);
    server.onNotFound(handleNotFound);
    server.begin();
  }

  void update()
  {
    if (!ready)
    {
      return;
    }

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
    {
      return "0.0.0.0";
    }

    return ::WiFi.softAPIP().toString();
  }
}
