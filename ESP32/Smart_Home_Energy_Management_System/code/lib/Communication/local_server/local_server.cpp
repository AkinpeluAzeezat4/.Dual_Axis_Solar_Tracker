#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "local_server.h"
#include "config_manager/config_manager.h"
#include "load_relay/load_relay.h"
#include "pzem_sensor/pzem_sensor.h"
#include "load_manager/load_manager.h"
#include "nepa_sense/nepa_sense.h"
#include "inverter_sense/inverter_sense.h"

namespace local_server
{
  WebServer server(80);

  const char *ssid = "SHEMS-Controller";
  const char *password = "12345678";

  String ipText = "0.0.0.0";

  String relayJson()
  {
    String data = "[";

    for (int i = 0; i < 6; i++)
    {
      if (i > 0)
        data += ",";

      data += "{\"id\":";
      data += String(i + 1);
      data += ",\"power\":";
      data += String(config_manager::getRelayPower(i));
      data += ",\"source\":\"";
      data += (load_relay::isOnInverter(i) ? "INVERTER" : "PHCN");
      data += "\"}";
    }

    data += "]";
    return data;
  }

  String statusJson()
  {
    String data = "{";

    data += "\"voltage\":";
    data += String(pzem_sensor::getVoltage(), 1);

    data += ",\"current\":";
    data += String(pzem_sensor::getCurrent(), 2);

    data += ",\"power\":";
    data += String(pzem_sensor::getPower(), 0);

    data += ",\"energy\":";
    data += String(pzem_sensor::getEnergy(), 0);

    data += ",\"inverterPower\":";
    data += String(config_manager::getInverterPower());

    data += ",\"systemPower\":";
    data += String(config_manager::getSystemPower());

    data += ",\"effectiveLimit\":";
    data += String(load_manager::getEffectiveLimit());

    data += ",\"loadRatio\":";
    data += String(load_manager::getLoadRatio(), 2);

    data += ",\"fuzzyRisk\":";
    data += String(load_manager::getFuzzyRisk(), 2);

    data += ",\"decision\":\"";
    data += load_manager::getDecisionText();

    data += "\",\"phcn\":";
    data += (nepa_sense::isAvailable() ? "true" : "false");

    data += ",\"inverter\":";
    data += (inverter_sense::isAvailable() ? "true" : "false");

    data += ",\"relays\":";
    data += relayJson();

    data += "}";

    return data;
  }

  void sendHome()
  {
    String html;
    html.reserve(9000);

    html += "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>";
    html += "<title>SHEMS</title>";
    html += "<style>";
    html += "body{font-family:Arial;margin:0;background:#101820;color:#f4f4f4}";
    html += ".wrap{padding:16px;max-width:900px;margin:auto}";
    html += ".card{background:#1e2a35;border-radius:14px;padding:14px;margin:10px 0;box-shadow:0 3px 10px #0008}";
    html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(140px,1fr));gap:10px}";
    html += ".v{font-size:22px;font-weight:700}";
    html += ".tag{display:inline-block;padding:4px 8px;border-radius:20px;background:#34495e;margin:2px}";
    html += "input{width:90px;padding:6px;border-radius:8px;border:0}";
    html += "button{padding:8px 12px;border:0;border-radius:8px;margin-top:8px;font-weight:bold}";
    html += ".inv{color:#7bed9f}.phcn{color:#ffdd59}";
    html += "</style></head><body><div class='wrap'>";

    html += "<h2>Smart Home Energy Management System</h2>";

    html += "<div class='card'><div id='decision'>Loading...</div></div>";

    html += "<div class='grid'>";
    html += "<div class='card'>Voltage<div class='v' id='voltage'></div></div>";
    html += "<div class='card'>Current<div class='v' id='current'></div></div>";
    html += "<div class='card'>Power<div class='v' id='power'></div></div>";
    html += "<div class='card'>Energy<div class='v' id='energy'></div></div>";
    html += "</div>";

    html += "<div class='card'><h3>Relays</h3><div id='relays'></div></div>";

    html += "<div class='card'><h3>Setup</h3><form action='/save' method='get'>";

    html += "Inverter Power <input name='inv' value='";
    html += String(config_manager::getInverterPower());
    html += "'> W<br><br>";

    html += "System Power <input name='sys' value='";
    html += String(config_manager::getSystemPower());
    html += "'> W<br><br>";

    html += "Load Margin <input name='margin' value='";
    html += String(config_manager::getLoadMarginPercent());
    html += "'> %<br><br>";

    for (int i = 0; i < 6; i++)
    {
      html += "Relay ";
      html += String(i + 1);
      html += " <input name='r";
      html += String(i + 1);
      html += "' value='";
      html += String(config_manager::getRelayPower(i));
      html += "'> W<br><br>";
    }

    html += "<button type='submit'>Save Setup</button></form></div>";

    html += "</div>";

    html += "<script>";
    html += "async function refresh(){";
    html += "let r=await fetch('/api/status');";
    html += "let d=await r.json();";
    html += "document.getElementById('voltage').innerText=d.voltage+' V';";
    html += "document.getElementById('current').innerText=d.current+' A';";
    html += "document.getElementById('power').innerText=d.power+' W';";
    html += "document.getElementById('energy').innerText=d.energy+' Wh';";
    html += "document.getElementById('decision').innerHTML='<b>'+d.decision+'</b> | Ratio '+d.loadRatio+' | Fuzzy '+d.fuzzyRisk+' | PHCN '+d.phcn+' | Inverter '+d.inverter;";
    html += "let h='';";
    html += "d.relays.forEach(x=>{h+='<span class=tag>Relay '+x.id+': '+x.power+'W - <b class='+(x.source=='INVERTER'?'inv':'phcn')+'>'+x.source+'</b></span> ';});";
    html += "document.getElementById('relays').innerHTML=h;";
    html += "}";
    html += "setInterval(refresh,2000);";
    html += "refresh();";
    html += "</script>";

    html += "</body></html>";

    server.send(200, "text/html", html);
  }

  void handleSave()
  {
    if (server.hasArg("inv"))
      config_manager::setInverterPower(server.arg("inv").toInt());

    if (server.hasArg("sys"))
      config_manager::setSystemPower(server.arg("sys").toInt());

    if (server.hasArg("margin"))
      config_manager::setLoadMarginPercent(server.arg("margin").toInt());

    for (int i = 0; i < 6; i++)
    {
      String key = "r";
      key += String(i + 1);

      if (server.hasArg(key))
        config_manager::setRelayPower(i, server.arg(key).toInt());
    }

    config_manager::save();

    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "Saved");
  }

  void begin()
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    ipText = WiFi.softAPIP().toString();

    server.on("/", sendHome);
    server.on("/save", handleSave);

    server.on("/api/status", []()
              { server.send(200, "application/json", statusJson()); });

    server.onNotFound([]()
                      { server.send(404, "text/plain", "Not found"); });

    server.begin();
  }

  void update()
  {
    server.handleClient();
  }

  const char *getIpAddress()
  {
    return ipText.c_str();
  }
}
