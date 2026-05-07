#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include "local_server.h"
#include "config_manager/config_manager.h"
#include "pzem_sensor/pzem_sensor.h"
#include "load_manager/load_manager.h"
#include "nepa_sense/nepa_sense.h"
#include "inverter_sense/inverter_sense.h"

namespace local_server
{
  WebServer server(80);

  const char *apSsid = "SHEMS-Controller";
  const char *apPassword = "12345678";

  String ipAddress = "0.0.0.0";

  String sourceText(bool state)
  {
    return state ? "AVAILABLE" : "NOT AVAILABLE";
  }

  String relaySourceText(bool state)
  {
    return state ? "INVERTER" : "PHCN";
  }

  String statusJson()
  {
    String data = "{";

    data += "\"voltage\":";
    data += String(pzem_sensor::getVoltage(), 1);
    data += ",";

    data += "\"current\":";
    data += String(pzem_sensor::getCurrent(), 2);
    data += ",";

    data += "\"power\":";
    data += String(pzem_sensor::getPower(), 0);
    data += ",";

    data += "\"energy\":";
    data += String(pzem_sensor::getEnergy(), 2);
    data += ",";

    data += "\"totalPower\":";
    data += String(pzem_sensor::getTotalPower());
    data += ",";

    data += "\"inverterPower\":";
    data += String(config_manager::getInverterPower());
    data += ",";

    data += "\"effectiveLimit\":";
    data += String(load_manager::getEffectiveLimit());
    data += ",";

    data += "\"loadMargin\":";
    data += String(config_manager::getLoadMarginPercent());
    data += ",";

    data += "\"loadRatio\":";
    data += String(load_manager::getLoadRatio(), 2);
    data += ",";

    data += "\"fuzzyRisk\":";
    data += String(load_manager::getFuzzyRisk(), 2);
    data += ",";

    data += "\"nepa\":\"";
    data += sourceText(nepa_sense::isAvailable());
    data += "\",";

    data += "\"inverter\":\"";
    data += sourceText(inverter_sense::isAvailable());
    data += "\",";

    data += "\"decision\":\"";
    data += load_manager::getDecisionText();
    data += "\",";

    data += "\"relays\":[";

    for (int i = 0; i < 6; i++)
    {
      data += "{";
      data += "\"id\":";
      data += String(i + 1);
      data += ",";
      data += "\"power\":";
      data += String(config_manager::getRelayPower(i));
      data += ",";
      data += "\"source\":\"";
      data += relaySourceText(load_manager::isOnInverter(i));
      data += "\"";
      data += "}";

      if (i < 5)
        data += ",";
    }

    data += "]";
    data += "}";

    return data;
  }

  void sendStatus()
  {
    server.send(200, "application/json", statusJson());
  }

  void sendHome()
  {
    String html;

    html += "<!DOCTYPE html>";
    html += "<html>";
    html += "<head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "<title>Smart Home Energy Management System</title>";

    html += "<style>";
    html += "body{font-family:Arial;background:#f2f4f8;margin:0;padding:0;color:#111;}";
    html += ".wrap{max-width:900px;margin:auto;padding:16px;}";
    html += ".card{background:#fff;border-radius:14px;padding:16px;margin:12px 0;box-shadow:0 2px 8px rgba(0,0,0,.12);}";
    html += "h1{font-size:22px;margin:8px 0 14px;}";
    html += "h2{font-size:17px;margin:4px 0 12px;}";
    html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(140px,1fr));gap:10px;}";
    html += ".box{background:#eef2ff;border-radius:10px;padding:12px;}";
    html += ".label{font-size:12px;color:#555;}";
    html += ".value{font-size:20px;font-weight:bold;margin-top:4px;}";
    html += "input{width:100%;padding:9px;border:1px solid #bbb;border-radius:8px;box-sizing:border-box;}";
    html += "button{padding:10px 14px;border:0;border-radius:8px;background:#111;color:#fff;font-weight:bold;}";
    html += "table{width:100%;border-collapse:collapse;}";
    html += "td,th{padding:8px;border-bottom:1px solid #ddd;text-align:left;}";
    html += ".ok{color:green;font-weight:bold;}";
    html += ".bad{color:red;font-weight:bold;}";
    html += "</style>";

    html += "</head>";
    html += "<body>";
    html += "<div class='wrap'>";

    html += "<h1>Smart Home Energy Management System</h1>";

    html += "<div class='card'>";
    html += "<h2>Live Sensor Readings</h2>";
    html += "<div class='grid'>";

    html += "<div class='box'><div class='label'>Voltage</div><div class='value' id='voltage'>0V</div></div>";
    html += "<div class='box'><div class='label'>Current</div><div class='value' id='current'>0A</div></div>";
    html += "<div class='box'><div class='label'>Actual Load Power</div><div class='value' id='power'>0W</div></div>";
    html += "<div class='box'><div class='label'>Energy</div><div class='value' id='energy'>0Wh</div></div>";

    html += "</div>";
    html += "</div>";

    html += "<div class='card'>";
    html += "<h2>System Decision</h2>";
    html += "<div class='grid'>";

    html += "<div class='box'><div class='label'>NEPA / PHCN</div><div class='value' id='nepa'>---</div></div>";
    html += "<div class='box'><div class='label'>Inverter</div><div class='value' id='inverter'>---</div></div>";
    html += "<div class='box'><div class='label'>Effective Limit</div><div class='value' id='effectiveLimit'>0W</div></div>";
    html += "<div class='box'><div class='label'>Decision</div><div class='value' id='decision'>---</div></div>";

    html += "</div>";
    html += "</div>";

    html += "<div class='card'>";
    html += "<h2>Load Configuration</h2>";
    html += "<form action='/save' method='GET'>";

    html += "<div class='grid'>";

    html += "<div class='box'>";
    html += "<div class='label'>Inverter Power Rating W</div>";
    html += "<input name='inv' type='number' min='500' max='20000' value='";
    html += String(config_manager::getInverterPower());
    html += "'>";
    html += "</div>";

    html += "<div class='box'>";
    html += "<div class='label'>Allowed Load Margin %</div>";
    html += "<input name='margin' type='number' min='50' max='100' value='";
    html += String(config_manager::getLoadMarginPercent());
    html += "'>";
    html += "</div>";

    for (int i = 0; i < 6; i++)
    {
      html += "<div class='box'>";
      html += "<div class='label'>Relay ";
      html += String(i + 1);
      html += " Load Power W</div>";
      html += "<input name='r";
      html += String(i + 1);
      html += "' type='number' min='0' max='4000' value='";
      html += String(config_manager::getRelayPower(i));
      html += "'>";
      html += "</div>";
    }

    html += "</div>";
    html += "<br>";
    html += "<button type='submit'>Save Settings</button>";
    html += "</form>";
    html += "</div>";

    html += "<div class='card'>";
    html += "<h2>Relay Source Status</h2>";
    html += "<table>";
    html += "<thead><tr><th>Relay</th><th>Configured Load</th><th>Active Source</th></tr></thead>";
    html += "<tbody id='relayTable'></tbody>";
    html += "</table>";
    html += "</div>";

    html += "</div>";

    html += "<script>";
    html += "function setText(id,text){document.getElementById(id).innerText=text;}";
    html += "function statusClass(text){return text==='AVAILABLE'?'ok':'bad';}";
    html += "function refresh(){";
    html += "fetch('/status').then(r=>r.json()).then(d=>{";
    html += "setText('voltage',Number(d.voltage).toFixed(1)+'V');";
    html += "setText('current',Number(d.current).toFixed(2)+'A');";
    html += "setText('power',Number(d.power).toFixed(0)+'W');";
    html += "setText('energy',Number(d.energy).toFixed(2)+'Wh');";
    html += "setText('effectiveLimit',d.effectiveLimit+'W');";
    html += "setText('decision',d.decision);";
    html += "setText('nepa',d.nepa);";
    html += "setText('inverter',d.inverter);";
    html += "document.getElementById('nepa').className='value '+statusClass(d.nepa);";
    html += "document.getElementById('inverter').className='value '+statusClass(d.inverter);";
    html += "let rows='';";
    html += "for(let i=0;i<d.relays.length;i++){";
    html += "let r=d.relays[i];";
    html += "rows += '<tr><td>Relay '+r.id+'</td><td>'+r.power+'W</td><td>'+r.source+'</td></tr>';";
    html += "}";
    html += "document.getElementById('relayTable').innerHTML=rows;";
    html += "});";
    html += "}";
    html += "setInterval(refresh,2000);";
    html += "refresh();";
    html += "</script>";

    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
  }

  void handleSave()
  {
    if (server.hasArg("inv"))
      config_manager::setInverterPower(server.arg("inv").toInt());

    if (server.hasArg("margin"))
      config_manager::setLoadMarginPercent(server.arg("margin").toInt());

    for (int i = 0; i < 6; i++)
    {
      String key = "r" + String(i + 1);

      if (server.hasArg(key))
        config_manager::setRelayPower(i, server.arg(key).toInt());
    }

    config_manager::save();

    server.sendHeader("Location", "/");
    server.send(303);
  }

  void begin()
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPassword);

    ipAddress = WiFi.softAPIP().toString();

    server.on("/", sendHome);
    server.on("/status", sendStatus);
    server.on("/save", handleSave);

    server.begin();
  }

  void update()
  {
    server.handleClient();
  }

  const char *getIpAddress()
  {
    return ipAddress.c_str();
  }
}
