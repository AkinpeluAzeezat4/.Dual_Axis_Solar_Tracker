#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "local_server.h"
#include "config_manager/config_manager.h"
#include "pzem_sensor/pzem_sensor.h"
#include "load_manager/load_manager.h"
#include "load_relay/load_relay.h"
#include "nepa_sense/nepa_sense.h"
#include "inverter_sense/inverter_sense.h"
#include "gpio_expander/gpio_expander.h"

namespace local_server
{
  WebServer server(80);

  const char *apSsid = "SHEMS-Controller";
  const char *apPassword = "12345678";
  char ipText[20] = "0.0.0.0";

  bool ready = false;

  int toIntArg(const String &name, int fallback)
  {
    if (!server.hasArg(name))
      return fallback;

    return server.arg(name).toInt();
  }

  String sourceText(bool onInverter, bool enabled)
  {
    if (!enabled)
      return "OFF";

    if (onInverter)
      return "INVERTER";

    return "GRID";
  }

  void handleApiStatus()
  {
    String json;
    json.reserve(900);

    json += "{";
    json += "\"voltage\":" + String(pzem_sensor::getVoltage(), 1) + ",";
    json += "\"current\":" + String(pzem_sensor::getCurrent(), 2) + ",";
    json += "\"power\":" + String(pzem_sensor::getPower(), 1) + ",";
    json += "\"energy\":" + String(pzem_sensor::getEnergy(), 3) + ",";
    json += "\"frequency\":" + String(pzem_sensor::getFrequency(), 1) + ",";
    json += "\"pf\":" + String(pzem_sensor::getPowerFactor(), 2) + ",";
    json += "\"pzemValid\":" + String(pzem_sensor::hasValidData() ? "true" : "false") + ",";
    json += "\"nepaAvailable\":" + String(nepa_sense::isAvailable() ? "true" : "false") + ",";
    json += "\"inverterAvailable\":" + String(inverter_sense::isAvailable() ? "true" : "false") + ",";
    json += "\"expanderReady\":" + String(gpio_expander::isReady() ? "true" : "false") + ",";
    json += "\"currentLoad\":" + String(load_manager::getCurrentLoad()) + ",";
    json += "\"effectiveLimit\":" + String(load_manager::getEffectiveLimit()) + ",";
    json += "\"loadRatio\":" + String(load_manager::getLoadRatio(), 3) + ",";
    json += "\"fuzzyRisk\":" + String(load_manager::getFuzzyRisk(), 3) + ",";
    json += "\"decision\":\"" + String(load_manager::getDecisionText()) + "\",";
    json += "\"inverterPower\":" + String(config_manager::getInverterPower()) + ",";
    json += "\"loadMargin\":" + String(config_manager::getLoadMarginPercent()) + ",";
    json += "\"loads\":[";

    for (int i = 0; i < 6; i++)
    {
      if (i > 0)
        json += ",";

      json += "{";
      json += "\"id\":" + String(i + 1) + ",";
      json += "\"configuredPower\":" + String(config_manager::getRelayPower(i)) + ",";
      json += "\"enabled\":" + String(load_relay::isLoadEnabled(i) ? "true" : "false") + ",";
      json += "\"source\":\"" + sourceText(load_relay::isOnInverter(i), load_relay::isLoadEnabled(i)) + "\"";
      json += "}";
    }

    json += "]";
    json += "}";

    server.send(200, "application/json", json);
  }

  void handleSaveConfig()
  {
    for (int i = 0; i < 6; i++)
    {
      String name = "r" + String(i + 1);
      config_manager::setRelayPower(i, toIntArg(name, config_manager::getRelayPower(i)));
    }

    config_manager::setInverterPower(toIntArg("inv", config_manager::getInverterPower()));
    config_manager::setLoadMarginPercent(toIntArg("margin", config_manager::getLoadMarginPercent()));
    config_manager::save();

    server.sendHeader("Location", "/");
    server.send(303);
  }

  void handleResetEnergy()
  {
    pzem_sensor::resetEnergy();
    server.sendHeader("Location", "/");
    server.send(303);
  }

  void handleDefaults()
  {
    config_manager::resetDefaults();
    server.sendHeader("Location", "/");
    server.send(303);
  }

  void handleRoot()
  {
    String html;
    html.reserve(6500);

    html += "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>SHEMS</title>";
    html += "<style>body{font-family:Arial;margin:15px;background:#f2f2f2;color:#111}h2{margin:0 0 10px}.card{background:#fff;padding:14px;margin:10px 0;border-radius:10px;box-shadow:0 1px 4px #bbb}.grid{display:grid;grid-template-columns:1fr 1fr;gap:8px}.v{font-weight:bold}.ok{color:green}.bad{color:#b00020}input{width:100%;padding:8px;margin:4px 0 10px;box-sizing:border-box}button{padding:10px 14px;border:0;border-radius:8px;background:#111;color:#fff}.small{font-size:12px;color:#555}table{width:100%;border-collapse:collapse}td,th{padding:6px;border-bottom:1px solid #ddd;text-align:left}</style>";
    html += "</head><body>";
    html += "<h2>Smart Home Energy Management System</h2>";

    html += "<div class='card'><div class='grid'>";
    html += "<div>Voltage<br><span class='v'>" + String(pzem_sensor::getVoltage(), 1) + " V</span></div>";
    html += "<div>Current<br><span class='v'>" + String(pzem_sensor::getCurrent(), 2) + " A</span></div>";
    html += "<div>Power<br><span class='v'>" + String(pzem_sensor::getPower(), 1) + " W</span></div>";
    html += "<div>Energy<br><span class='v'>" + String(pzem_sensor::getEnergy(), 3) + " kWh</span></div>";
    html += "</div></div>";

    html += "<div class='card'>";
    html += "Decision: <b>" + String(load_manager::getDecisionText()) + "</b><br>";
    html += "NEPA/Grid: <b class='" + String(nepa_sense::isAvailable() ? "ok" : "bad") + "'>" + String(nepa_sense::isAvailable() ? "AVAILABLE" : "OFF") + "</b><br>";
    html += "Inverter: <b class='" + String(inverter_sense::isAvailable() ? "ok" : "bad") + "'>" + String(inverter_sense::isAvailable() ? "AVAILABLE" : "OFF") + "</b><br>";
    html += "PZEM: <b class='" + String(pzem_sensor::hasValidData() ? "ok" : "bad") + "'>" + String(pzem_sensor::hasValidData() ? "VALID" : "ERROR") + "</b><br>";
    html += "PCA9555: <b class='" + String(gpio_expander::isReady() ? "ok" : "bad") + "'>" + String(gpio_expander::isReady() ? "READY" : "ERROR") + "</b><br>";
    html += "Effective inverter limit: <b>" + String(load_manager::getEffectiveLimit()) + " W</b>";
    html += "</div>";

    html += "<div class='card'><table><tr><th>Load</th><th>Power</th><th>Status</th></tr>";
    for (int i = 0; i < 6; i++)
    {
      html += "<tr><td>Load " + String(i + 1) + "</td><td>" + String(config_manager::getRelayPower(i)) + " W</td><td>" + sourceText(load_relay::isOnInverter(i), load_relay::isLoadEnabled(i)) + "</td></tr>";
    }
    html += "</table></div>";

    html += "<div class='card'><form action='/save' method='get'>";
    html += "<h3>Configuration</h3>";
    for (int i = 0; i < 6; i++)
    {
      html += "Load " + String(i + 1) + " Power (W)<input name='r" + String(i + 1) + "' type='number' value='" + String(config_manager::getRelayPower(i)) + "'>";
    }
    html += "Inverter Power (W)<input name='inv' type='number' value='" + String(config_manager::getInverterPower()) + "'>";
    html += "Load Margin (%)<input name='margin' type='number' value='" + String(config_manager::getLoadMarginPercent()) + "'>";
    html += "<button type='submit'>Save Configuration</button>";
    html += "</form></div>";

    html += "<div class='card'><a href='/api/status'>API Status</a><br><br><a href='/resetEnergy'>Reset PZEM Energy</a><br><br><a href='/defaults'>Restore Defaults</a></div>";
    html += "<div class='small'>Connect to WiFi: SHEMS-Controller, Password: 12345678, IP: " + String(ipText) + "</div>";
    html += "</body></html>";

    server.send(200, "text/html", html);
  }

  void begin()
  {
    WiFi.mode(WIFI_AP);
    bool apOk = WiFi.softAP(apSsid, apPassword);

    IPAddress ip = WiFi.softAPIP();
    snprintf(ipText, sizeof(ipText), "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

    server.on("/", handleRoot);
    server.on("/api/status", handleApiStatus);
    server.on("/save", handleSaveConfig);
    server.on("/resetEnergy", handleResetEnergy);
    server.on("/defaults", handleDefaults);

    server.begin();

    ready = apOk;
  }

  void update()
  {
    server.handleClient();
  }

  const char *getIpAddress()
  {
    return ipText;
  }

  bool isReady()
  {
    return ready;
  }
}
