#include <Arduino.h>
#include <WebServer.h>
#include "web_dashboard.h"
#include "wifi_manager.h"
#include "battery_level.h"
#include "solar_level.h"
#include "temp_hum.h"
#include "ultrasonic.h"
#include "heater.h"
#include "spinner.h"
#include "humidifier.h"
#include "buzzer.h"
#include "lcd_screen.h"
#include "rtc_clock.h"

namespace web_dashboard
{
  static WebServer server(80);

  static String relayState(bool state)
  {
    return state ? "ON" : "OFF";
  }

  static String page()
  {
    String html;
    html.reserve(7000);

    html += "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'><title>Egg Incubator</title>";
    html += "<style>body{font-family:Arial;background:#f3f4f6;margin:0;color:#111827}header{background:#111827;color:white;padding:16px}main{padding:14px;max-width:900px;margin:auto}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:12px}.card{background:white;border-radius:16px;padding:14px;box-shadow:0 4px 14px #0001}.v{font-size:26px;font-weight:700;margin-top:8px}.btn{display:inline-block;padding:10px 14px;margin:4px;border-radius:10px;text-decoration:none;color:white;background:#2563eb}.off{background:#6b7280}.danger{background:#dc2626}</style>";
    html += "<script>setInterval(()=>fetch('/api').then(r=>r.json()).then(d=>{for(let k in d){let e=document.getElementById(k);if(e)e.innerHTML=d[k];}}),1000)</script></head><body>";

    html += "<header><h2>Solar Powered Egg Incubator</h2><div>IP: <span id='ip'>" + wifi_manager::getIp() + "</span> | Time: <span id='time'>" + rtc_clock::getDateTime() + "</span></div></header>";
    html += "<main><div class='grid'>";

    html += "<div class='card'>Temperature<div class='v'><span id='temp'>" + String(temp_hum::getTemperature(), 1) + "</span> &deg;C</div></div>";
    html += "<div class='card'>Humidity<div class='v'><span id='hum'>" + String(temp_hum::getHumidity(), 0) + "</span> %</div></div>";
    html += "<div class='card'>Battery<div class='v'><span id='battv'>" + String(battery_level::getVoltage(), 2) + "</span> V</div><span id='battp'>" + String(battery_level::getPercentage()) + "</span>%</div>";
    html += "<div class='card'>Solar<div class='v'><span id='solarv'>" + String(solar_level::getVoltage(), 2) + "</span> V</div><span id='solarp'>" + String(solar_level::getPercentage()) + "</span>%</div>";
    html += "<div class='card'>Ultrasonic<div class='v'><span id='dist'>" + String(ultrasonic::getDistanceCm(), 1) + "</span> cm</div></div>";
    html += "<div class='card'>LCD<div class='v'><span id='lcd'>" + String(lcd_screen::isAwake() ? "AWAKE" : "SLEEP") + "</span></div></div>";

    html += "</div><h3>Controls</h3><div class='grid'>";

    html += "<div class='card'>Heater: <b id='heater'>" + relayState(heater::isOn()) + "</b><br><a class='btn' href='/toggle?dev=heater'>Toggle</a></div>";
    html += "<div class='card'>Spinner: <b id='spinner'>" + relayState(spinner::isOn()) + "</b><br><a class='btn' href='/toggle?dev=spinner'>Toggle</a></div>";
    html += "<div class='card'>Humidifier: <b id='humidifier'>" + relayState(humidifier::isOn()) + "</b><br><a class='btn' href='/toggle?dev=humidifier'>Toggle</a></div>";
    html += "<div class='card'>Buzzer<br><a class='btn danger' href='/beep'>Beep</a></div>";
    html += "<div class='card'>LCD<br><a class='btn' href='/lcd?wake=1'>Wake</a><a class='btn off' href='/lcd?wake=0'>Sleep</a></div>";

    html += "</div></main></body></html>";

    return html;
  }

  static void sendApi()
  {
    String json = "{";

    json += "\"ip\":\"" + wifi_manager::getIp() + "\",";
    json += "\"time\":\"" + rtc_clock::getDateTime() + "\",";
    json += "\"temp\":\"" + String(temp_hum::getTemperature(), 1) + "\",";
    json += "\"hum\":\"" + String(temp_hum::getHumidity(), 0) + "\",";
    json += "\"battv\":\"" + String(battery_level::getVoltage(), 2) + "\",";
    json += "\"battp\":\"" + String(battery_level::getPercentage()) + "\",";
    json += "\"solarv\":\"" + String(solar_level::getVoltage(), 2) + "\",";
    json += "\"solarp\":\"" + String(solar_level::getPercentage()) + "\",";
    json += "\"dist\":\"" + String(ultrasonic::getDistanceCm(), 1) + "\",";
    json += "\"lcd\":\"" + String(lcd_screen::isAwake() ? "AWAKE" : "SLEEP") + "\",";
    json += "\"heater\":\"" + relayState(heater::isOn()) + "\",";
    json += "\"spinner\":\"" + relayState(spinner::isOn()) + "\",";
    json += "\"humidifier\":\"" + relayState(humidifier::isOn()) + "\"";

    json += "}";

    server.send(200, "application/json", json);
  }

  void begin()
  {
    server.on("/", []()
              { server.send(200, "text/html", page()); });

    server.on("/api", []()
              { sendApi(); });

    server.on("/toggle", []()
              {
                String dev = server.arg("dev");

                if (dev == "heater")
                {
                  heater::toggle();
                }
                else if (dev == "spinner")
                {
                  spinner::toggle();
                }
                else if (dev == "humidifier")
                {
                  humidifier::toggle();
                }

                server.sendHeader("Location", "/");
                server.send(303); });

    server.on("/beep", []()
              {
                buzzer::beep(100, 100, 2);
                server.sendHeader("Location", "/");
                server.send(303); });

    server.on("/lcd", []()
              {
                if (server.arg("wake") == "1")
                {
                  lcd_screen::wake();
                }
                else
                {
                  lcd_screen::sleep();
                }

                server.sendHeader("Location", "/");
                server.send(303); });

    server.begin();
  }

  void update()
  {
    server.handleClient();
  }
}
