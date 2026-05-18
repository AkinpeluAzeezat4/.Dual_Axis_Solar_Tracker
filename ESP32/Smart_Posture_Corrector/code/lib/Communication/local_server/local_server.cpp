#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SD.h>
#include "local_server.h"
#include "mpu6050_sensor/mpu6050_sensor.h"
#include "battery_level/battery_level.h"
#include "posture_logic/posture_logic.h"
#include "sd_card/sd_card.h"
#include "vibration_motor/vibration_motor.h"
#include "buzzer/buzzer.h"

namespace local_server
{
  static WebServer server(80);

  static const char *apSsid = "Posture-Corrector";
  static const char *apPassword = "12345678";

  static String pageHeader(const String &title)
  {
    String html;

    html += F("<!DOCTYPE html><html lang='en'><head>");
    html += F("<meta charset='UTF-8'>");
    html += F("<meta name='viewport' content='width=device-width,initial-scale=1.0'>");
    html += F("<title>");
    html += title;
    html += F("</title>");

    html += F("<style>");
    html += F(":root{--bg:#eef2f7;--card:#fff;--text:#172033;--muted:#667085;--box:#f4f7fb;--border:#d9e2ef;--primary:#0b5fff;--primary-dark:#0845b8;--danger:#d92d20;--warning:#f79009;--success:#039855;--shadow:0 12px 35px rgba(20,35,65,.12)}");
    html += F("body.dark{--bg:#0b1220;--card:#111827;--text:#f8fafc;--muted:#94a3b8;--box:#1f2937;--border:#334155;--primary:#3b82f6;--primary-dark:#2563eb;--danger:#ef4444;--warning:#f59e0b;--success:#22c55e;--shadow:0 12px 35px rgba(0,0,0,.35)}");
    html += F("*{box-sizing:border-box}");
    html += F("body{margin:0;min-height:100vh;font-family:Arial,Helvetica,sans-serif;background:var(--bg);color:var(--text);padding:16px;transition:.25s}");
    html += F(".page{width:100%;max-width:1050px;margin:0 auto;display:flex;flex-direction:column;gap:16px}");
    html += F(".card{background:var(--card);border:1px solid var(--border);border-radius:18px;box-shadow:var(--shadow);padding:18px}");
    html += F(".topbar{display:flex;justify-content:space-between;align-items:center;gap:12px;flex-wrap:wrap}");
    html += F(".title h1{font-size:clamp(24px,5vw,38px);margin:0 0 6px;line-height:1.1}");
    html += F(".title p{margin:0;color:var(--muted);font-size:14px}");
    html += F(".theme-btn{border:1px solid var(--border);background:var(--box);color:var(--text);padding:10px 14px;border-radius:999px;cursor:pointer;font-weight:700}");
    html += F(".hero-grid{display:grid;grid-template-columns:1.15fr .85fr;gap:16px}");
    html += F(".state-panel{display:flex;flex-direction:column;align-items:center;justify-content:center;text-align:center;min-height:230px}");
    html += F(".state-circle{width:150px;height:150px;border-radius:50%;display:flex;align-items:center;justify-content:center;background:rgba(3,152,85,.13);border:8px solid var(--success);margin-bottom:16px}");
    html += F(".state-text{font-size:26px;font-weight:800;color:var(--success)}");
    html += F(".state-help{color:var(--muted);margin:0;font-size:14px}");
    html += F(".summary-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:12px}");
    html += F(".metric{background:var(--box);border:1px solid var(--border);border-radius:14px;padding:14px;min-height:92px}");
    html += F(".metric label{display:block;color:var(--muted);font-size:13px;margin-bottom:8px}");
    html += F(".metric span{display:block;font-size:clamp(20px,4vw,28px);font-weight:800}");
    html += F(".metric small{color:var(--muted);font-size:12px}");
    html += F(".battery-bar{width:100%;height:10px;background:var(--border);border-radius:20px;overflow:hidden;margin-top:10px}");
    html += F(".battery-fill{width:0%;height:100%;background:var(--success);border-radius:20px;transition:.25s}");
    html += F(".section-title{display:flex;justify-content:space-between;align-items:center;gap:10px;flex-wrap:wrap;margin-bottom:12px}");
    html += F(".section-title h2{margin:0;font-size:20px}");
    html += F(".status-pill{padding:7px 10px;border-radius:999px;font-size:12px;font-weight:800;background:rgba(3,152,85,.14);color:var(--success)}");
    html += F(".controls{display:grid;grid-template-columns:repeat(5,minmax(0,1fr));gap:10px}");
    html += F(".btn{display:flex;align-items:center;justify-content:center;min-height:44px;border:0;border-radius:12px;background:var(--primary);color:white;text-decoration:none;cursor:pointer;font-weight:800;font-size:14px;padding:10px;text-align:center}");
    html += F(".btn:hover{background:var(--primary-dark)}");
    html += F(".dark-btn{background:#344054}");
    html += F(".danger{background:var(--danger)}");
    html += F(".log-box{background:#06111f;color:#38ff88;padding:14px;border-radius:14px;overflow:auto;max-height:420px;font-size:13px;line-height:1.55;white-space:pre}");
    html += F(".footer-info{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px;color:var(--muted);font-size:13px}");
    html += F(".info-item{background:var(--box);border:1px solid var(--border);border-radius:12px;padding:12px}");
    html += F("@media(max-width:850px){.hero-grid{grid-template-columns:1fr}.controls{grid-template-columns:repeat(2,minmax(0,1fr))}.footer-info{grid-template-columns:1fr}}");
    html += F("@media(max-width:520px){body{padding:10px}.card{padding:14px;border-radius:15px}.summary-grid{grid-template-columns:1fr}.controls{grid-template-columns:1fr}.topbar{align-items:flex-start}.theme-btn{width:100%}.state-circle{width:125px;height:125px}}");
    html += F("</style>");

    html += F("</head><body><main class='page'>");

    return html;
  }

  static String pageFooter()
  {
    String html;

    html += F("</main>");
    html += F("<script>");
    html += F("let dark=localStorage.getItem('theme')==='dark';");
    html += F("function applyTheme(){document.body.classList.toggle('dark',dark);document.getElementById('themeBtn').innerText=dark?'Light Theme':'Dark Theme';localStorage.setItem('theme',dark?'dark':'light')}");
    html += F("function toggleTheme(){dark=!dark;applyTheme()}");
    html += F("function setStateUI(state,muted){const s=document.getElementById('state');const c=document.getElementById('stateCircle');const h=document.getElementById('stateHelp');if(!s)return;s.innerText=state;if(state==='GOOD'){s.style.color='var(--success)';c.style.borderColor='var(--success)';c.style.background='rgba(3,152,85,.13)';h.innerText='Your posture is currently within the calibrated safe range.'}else if(state==='BAD'){s.style.color='var(--warning)';c.style.borderColor='var(--warning)';c.style.background='rgba(247,144,9,.15)';h.innerText='Bad posture detected. Alert will trigger if it continues.'}else{s.style.color='var(--danger)';c.style.borderColor='var(--danger)';c.style.background='rgba(217,45,32,.15)';h.innerText='Posture alert is active. Please correct your posture.'}}");
    html += F("function refreshData(){fetch('/api').then(r=>r.json()).then(d=>{setStateUI(d.state,d.muted);document.getElementById('battery').innerText=d.battery_voltage+'V / '+d.battery_percent+'%';document.getElementById('batteryFill').style.width=d.battery_percent+'%';document.getElementById('mute').innerText=d.muted;document.getElementById('baseline').innerText=d.baseline+'°';document.getElementById('error').innerText=d.pitch_error+'°';document.getElementById('pitch').innerText=d.pitch+'°';document.getElementById('roll').innerText=d.roll+'°';document.getElementById('motor').innerText=d.motor;document.getElementById('buzzer').innerText=d.buzzer;document.getElementById('sdPill').innerText=d.sd==='OK'?'SD CARD OK':'SD CARD ERROR';});}");
    html += F("applyTheme();refreshData();setInterval(refreshData,1000);");
    html += F("</script></body></html>");

    return html;
  }

  static void handleRoot()
  {
    String html = pageHeader("Smart Posture Corrector");

    html += F("<section class='card topbar'>");
    html += F("<div class='title'><h1>Smart Posture Corrector</h1><p>ESP32 local dashboard for live posture monitoring and SD card log access.</p></div>");
    html += F("<button class='theme-btn' onclick='toggleTheme()' id='themeBtn'>Dark Theme</button>");
    html += F("</section>");

    html += F("<section class='hero-grid'>");
    html += F("<div class='card state-panel'>");
    html += F("<div class='state-circle' id='stateCircle'><div class='state-text' id='state'>---</div></div>");
    html += F("<p class='state-help' id='stateHelp'>Loading posture state...</p>");
    html += F("</div>");

    html += F("<div class='card'>");
    html += F("<div class='section-title'><h2>System Summary</h2><span class='status-pill' id='sdPill'>SD CARD</span></div>");
    html += F("<div class='summary-grid'>");
    html += F("<div class='metric'><label>Battery</label><span id='battery'>---</span><div class='battery-bar'><div class='battery-fill' id='batteryFill'></div></div></div>");
    html += F("<div class='metric'><label>Mute Status</label><span id='mute'>---</span><small>Short press button to toggle</small></div>");
    html += F("<div class='metric'><label>Baseline</label><span id='baseline'>---</span><small>Saved calibration angle</small></div>");
    html += F("<div class='metric'><label>Pitch Error</label><span id='error'>---</span><small>Current pitch minus baseline</small></div>");
    html += F("</div></div></section>");

    html += F("<section class='card'>");
    html += F("<div class='section-title'><h2>Live Sensor Values</h2><span class='status-pill'>AUTO REFRESH</span></div>");
    html += F("<div class='summary-grid'>");
    html += F("<div class='metric'><label>Pitch</label><span id='pitch'>---</span></div>");
    html += F("<div class='metric'><label>Roll</label><span id='roll'>---</span></div>");
    html += F("<div class='metric'><label>Vibration Motor</label><span id='motor'>---</span></div>");
    html += F("<div class='metric'><label>Buzzer</label><span id='buzzer'>---</span></div>");
    html += F("</div></section>");

    html += F("<section class='card'>");
    html += F("<div class='section-title'><h2>Controls</h2><span class='status-pill'>LOCAL CONTROL</span></div>");
    html += F("<div class='controls'>");
    html += F("<a class='btn' href='/calibrate'>Recalibrate</a>");
    html += F("<a class='btn dark-btn' href='/mute'>Mute / Unmute</a>");
    html += F("<a class='btn' href='/logs'>View Logs</a>");
    html += F("<a class='btn' href='/download'>Download CSV</a>");
    html += F("<a class='btn danger' href='/clear'>Clear Logs</a>");
    html += F("</div></section>");

    html += F("<section class='footer-info'>");
    html += F("<div class='info-item'><b>Wi-Fi SSID:</b><br>Posture-Corrector</div>");
    html += F("<div class='info-item'><b>Password:</b><br>*****</div>");
    html += F("<div class='info-item'><b>Dashboard IP:</b><br>192.168.4.1</div>");
    html += F("</section>");

    html += pageFooter();

    server.send(200, "text/html", html);
  }

  static void handleApi()
  {
    mpu6050_sensor::SensorData data = mpu6050_sensor::getData();

    String json = "{";
    json += "\"state\":\"" + String(posture_logic::getStateText()) + "\",";
    json += "\"pitch\":\"" + String(data.pitch, 2) + "\",";
    json += "\"roll\":\"" + String(data.roll, 2) + "\",";
    json += "\"pitch_error\":\"" + String(posture_logic::getPitchError(), 2) + "\",";
    json += "\"baseline\":\"" + String(posture_logic::getBaselinePitch(), 2) + "\",";
    json += "\"battery_voltage\":\"" + String(battery_level::getVoltage(), 2) + "\",";
    json += "\"battery_percent\":\"" + String(battery_level::getPercentage()) + "\",";
    json += "\"sd\":\"" + String(sd_card::isReady() ? "OK" : "NO") + "\",";
    json += "\"muted\":\"" + String(posture_logic::isMuted() ? "YES" : "NO") + "\",";
    json += "\"motor\":\"" + String(vibration_motor::isActive() ? "ON" : "OFF") + "\",";
    json += "\"buzzer\":\"" + String(buzzer::isActive() ? "ON" : "OFF") + "\"";
    json += "}";

    server.send(200, "application/json", json);
  }

  static void handleLogs()
  {
    String html = pageHeader("SD Card Logs");

    html += F("<section class='card topbar'>");
    html += F("<div class='title'><h1>SD Card Logs</h1><p>Reading posture_log.csv directly from the SD card backend.</p></div>");
    html += F("<button class='theme-btn' onclick='toggleTheme()' id='themeBtn'>Dark Theme</button>");
    html += F("</section>");

    html += F("<section class='card'>");
    html += F("<div class='section-title'><h2>/posture_log.csv</h2><span class='status-pill'>SD BACKEND</span></div>");
    html += F("<div class='controls'>");
    html += F("<a class='btn' href='/'>Dashboard</a>");
    html += F("<a class='btn' href='/download'>Download CSV</a>");
    html += F("<a class='btn danger' href='/clear'>Clear Logs</a>");
    html += F("</div><br>");
    html += F("<div class='log-box'>");
    html += sd_card::readLogFile();
    html += F("</div></section>");

    html += pageFooter();

    server.send(200, "text/html", html);
  }

  static void handleDownload()
  {
    if (!sd_card::isReady())
    {
      server.send(500, "text/plain", "SD card not ready");
      return;
    }

    File file = SD.open(sd_card::getLogFileName(), FILE_READ);

    if (!file)
    {
      server.send(404, "text/plain", "Log file not found");
      return;
    }

    server.sendHeader("Content-Disposition", "attachment; filename=posture_log.csv");
    server.streamFile(file, "text/csv");
    file.close();
  }

  static void handleClear()
  {
    sd_card::clearLogFile();
    server.sendHeader("Location", "/logs");
    server.send(303);
  }

  static void handleCalibrate()
  {
    posture_logic::requestCalibration();
    server.sendHeader("Location", "/");
    server.send(303);
  }

  static void handleMute()
  {
    posture_logic::setMuted(!posture_logic::isMuted());
    server.sendHeader("Location", "/");
    server.send(303);
  }

  void begin()
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPassword);

    server.on("/", handleRoot);
    server.on("/api", handleApi);
    server.on("/logs", handleLogs);
    server.on("/download", handleDownload);
    server.on("/clear", handleClear);
    server.on("/calibrate", handleCalibrate);
    server.on("/mute", handleMute);

    server.begin();
  }

  void update()
  {
    server.handleClient();
  }

  String getIP()
  {
    return WiFi.softAPIP().toString();
  }
}
