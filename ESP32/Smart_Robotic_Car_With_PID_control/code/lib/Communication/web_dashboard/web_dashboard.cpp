#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "web_dashboard.h"
#include "robot_control/robot_control.h"
#include "battery_level/battery_level.h"
#include "yahboom_motor/yahboom_motor.h"

namespace web_dashboard
{
  static WebServer server(80);

  static const char *apSsid = "SmartCar_PID";
  static const char *apPassword = "12345678";

  static String ipAddress = "0.0.0.0";

  static const char dashboardPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
<title>Smart Robotic Car</title>
<style>
:root{
  --bg:#0f1219;
  --bg2:#0b0d12;
  --panel:#1b2230;
  --panelTop:#20283a;
  --panel2:#1d2430;
  --text:#f3f6fb;
  --muted:#9aa6b2;
  --soft:#c7d0dc;
  --border:#2b3447;
  --blue:#2f6fed;
  --cyan:#1fa7c9;
  --green:#22a45a;
  --yellow:#d6a72d;
  --orange:#d9782f;
  --purple:#7b61ff;
  --red:#cc3344;
  --gray:#555d6d;
  --shadow:0 4px 18px #0006;
}
body.light{
  --bg:#eef3fb;
  --bg2:#dfe7f2;
  --panel:#ffffff;
  --panelTop:#f8fbff;
  --panel2:#ffffff;
  --text:#121826;
  --muted:#647084;
  --soft:#4d5b70;
  --border:#d9e2ef;
  --blue:#2563eb;
  --cyan:#0284a8;
  --green:#14934c;
  --yellow:#b88405;
  --orange:#c45f1f;
  --purple:#6750d8;
  --red:#c52b3d;
  --gray:#6b7280;
  --shadow:0 5px 18px #4b556322;
}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
html,body{min-height:100%;margin:0}
body{font-family:Arial,Helvetica,sans-serif;background:radial-gradient(circle at top,#172033 0%,var(--bg) 45%,var(--bg2) 100%);color:var(--text);text-align:center;overflow-x:hidden;transition:background .25s,color .25s}
body.light{background:radial-gradient(circle at top,#ffffff 0%,var(--bg) 48%,var(--bg2) 100%)}
header{width:100%;padding:clamp(16px,4vw,26px);background:linear-gradient(135deg,var(--panel2),var(--panel));text-align:center;border-bottom:1px solid var(--border);position:relative}
header h2{margin:0 0 6px;font-size:clamp(21px,4vw,30px);line-height:1.2;padding:0 72px}
.theme-toggle{position:absolute;right:14px;top:14px;display:flex;align-items:center;gap:7px;background:var(--panel);border:1px solid var(--border);border-radius:999px;padding:7px 9px;box-shadow:var(--shadow);color:var(--text)}
.theme-label{font-size:12px;font-weight:700;color:var(--soft)}
.switch{position:relative;width:44px;height:24px;display:inline-block}
.switch input{display:none}
.slider-toggle{position:absolute;cursor:pointer;inset:0;background:#394458;border-radius:999px;transition:.25s}
.slider-toggle:before{content:"";position:absolute;width:18px;height:18px;left:3px;top:3px;background:#fff;border-radius:50%;transition:.25s;box-shadow:0 2px 6px #0005}
.switch input:checked + .slider-toggle{background:var(--yellow)}
.switch input:checked + .slider-toggle:before{transform:translateX(20px)}
main{width:100%;padding:clamp(12px,3vw,20px);max-width:1120px;margin:0 auto;display:flex;flex-direction:column;align-items:center;gap:16px}
.small{font-size:clamp(13px,2.5vw,15px);color:var(--soft);line-height:1.4}
.grid{width:100%;display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:clamp(10px,2vw,14px)}
.card{width:100%;min-width:0;background:linear-gradient(180deg,var(--panelTop),var(--panel));border-radius:clamp(14px,3vw,20px);padding:clamp(12px,3vw,17px);box-shadow:var(--shadow);text-align:center;overflow:hidden;border:1px solid var(--border);position:relative}
.card::before{content:"";position:absolute;left:0;top:0;width:100%;height:4px;background:var(--accent,#3d4b63)}
.label{font-size:clamp(11px,2vw,13px);color:var(--muted);text-transform:uppercase;letter-spacing:.4px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.value{font-size:clamp(21px,4.6vw,30px);font-weight:800;margin-top:7px;line-height:1.15;word-break:break-word}
.mode-card{--accent:var(--green)}
.state-card{--accent:var(--purple)}
.distance-card{--accent:var(--cyan)}
.battery-card{--accent:var(--yellow)}
.speed-card-left{--accent:var(--blue)}
.speed-card-right{--accent:var(--blue)}
.scan-card{--accent:var(--orange)}
.gyro-card{--accent:var(--purple)}
.servo-card{--accent:var(--cyan)}
.motor-card{--accent:var(--green)}
.pid-card{--accent:var(--purple)}
.speed-card{max-width:560px;--accent:var(--blue)}
.slider{width:100%;max-width:520px;margin-top:12px;accent-color:var(--blue)}
.control-panel{width:100%;max-width:560px;background:linear-gradient(180deg,var(--panelTop),var(--panel));border-radius:clamp(16px,4vw,24px);padding:clamp(14px,3vw,20px);box-shadow:var(--shadow);border:1px solid var(--border)}
.control-title{font-size:clamp(14px,2.7vw,16px);color:var(--soft);margin-bottom:14px}
.drive-pad{width:100%;display:grid;grid-template-columns:repeat(3,minmax(0,1fr));grid-template-areas:"blank1 forward blank2" "left stop right" "blank3 backward blank4";gap:clamp(9px,2.5vw,13px);align-items:center;justify-items:center}
.btn{border:0;border-radius:clamp(13px,3vw,18px);padding:clamp(11px,3vw,16px) clamp(8px,2vw,14px);font-size:clamp(12px,2.6vw,15px);font-weight:800;background:var(--blue);color:white;cursor:pointer;min-height:clamp(50px,12vw,60px);width:100%;box-shadow:0 4px 12px #0005;touch-action:manipulation}
.btn:hover{filter:brightness(1.12)}
.btn:active{transform:scale(.98)}
.btn.forward{grid-area:forward;background:linear-gradient(135deg,var(--green),#168f63)}
body.light .btn.forward{background:linear-gradient(135deg,var(--green),#0f7b40)}
.btn.left{grid-area:left;background:linear-gradient(135deg,var(--blue),#2456c6)}
.btn.right{grid-area:right;background:linear-gradient(135deg,var(--blue),#2456c6)}
.btn.backward{grid-area:backward;background:linear-gradient(135deg,var(--orange),#b95d24)}
.btn.stop{grid-area:stop;background:linear-gradient(135deg,var(--red),#9d2433);border-radius:50%;width:clamp(64px,18vw,82px);height:clamp(64px,18vw,82px);min-height:clamp(64px,18vw,82px);padding:0;display:flex;align-items:center;justify-content:center}
.btn.auto{background:linear-gradient(135deg,var(--green),#168f63)}
.btn.idle{background:linear-gradient(135deg,var(--gray),#3f4654)}
.mode-row{display:grid;grid-template-columns:1fr 1fr;gap:clamp(9px,2.5vw,13px);margin-top:16px}
.footer-info{margin:2px 0 8px;text-align:center;max-width:100%;overflow-wrap:anywhere}
.arrow{display:block;font-size:clamp(20px,6vw,26px);line-height:1}
.btn-text{display:block;font-size:clamp(10px,2.3vw,12px);margin-top:4px;letter-spacing:.5px;white-space:nowrap}
@media(max-width:920px){.grid{grid-template-columns:repeat(3,minmax(0,1fr))}}
@media(max-width:700px){.grid{grid-template-columns:repeat(2,minmax(0,1fr))}main{gap:14px}.speed-card,.control-panel{max-width:100%}header h2{padding:0 64px;font-size:21px}.theme-toggle{right:10px;top:10px;padding:6px}.theme-label{display:none}}
@media(max-width:430px){header{padding:16px 10px 18px}header h2{padding:0 58px 0 0;text-align:left}.small{text-align:left}.theme-toggle{right:10px;top:12px}main{padding:10px;gap:12px}.grid{gap:9px}.card{padding:11px 8px;border-radius:14px}.label{font-size:10px;letter-spacing:.25px}.value{font-size:20px}.drive-pad{gap:8px}.control-panel{padding:12px;border-radius:17px}.btn{min-height:48px;border-radius:13px}.btn.stop{width:62px;height:62px;min-height:62px}.mode-row{grid-template-columns:1fr}.footer-info{font-size:12px}}
@media(max-width:340px){.grid{grid-template-columns:1fr}.drive-pad{grid-template-columns:1fr 1fr 1fr;gap:7px}.btn-text{font-size:9px}.btn{padding:9px 5px}.value{font-size:19px}.btn.stop{width:58px;height:58px;min-height:58px}}
@media(min-width:1200px){main{max-width:1180px}.grid{grid-template-columns:repeat(4,minmax(0,1fr))}}
</style>
</head>
<body>
<header>
<div class="theme-toggle"><span class="theme-label" id="themeText">Dark</span><label class="switch"><input id="themeSwitch" type="checkbox" onchange="toggleTheme()"><span class="slider-toggle"></span></label></div>
<h2>Smart Robotic Car Dashboard</h2><div class="small">ESP32 self-driving PID control</div></header>
<main>
<div class="grid">
<div class="card mode-card"><div class="label">Mode</div><div class="value" id="mode">--</div></div>
<div class="card state-card"><div class="label">Auto State</div><div class="value" id="state">--</div></div>
<div class="card distance-card"><div class="label">Front Distance</div><div class="value"><span id="distance">0.0</span> cm</div></div>
<div class="card battery-card"><div class="label">Battery</div><div class="value"><span id="battery">0</span>%</div><div class="small"><span id="voltage">0.00</span> V</div></div>
<div class="card speed-card-left"><div class="label">Left Speed</div><div class="value" id="leftSpeed">0</div></div>
<div class="card speed-card-right"><div class="label">Right Speed</div><div class="value" id="rightSpeed">0</div></div>
<div class="card scan-card"><div class="label">Right Scan</div><div class="value"><span id="rightDistance">0.0</span> cm</div></div>
<div class="card scan-card"><div class="label">Left Scan</div><div class="value"><span id="leftDistance">0.0</span> cm</div></div>
<div class="card gyro-card"><div class="label">Yaw</div><div class="value"><span id="yaw">0.0</span>&deg;</div></div>
<div class="card gyro-card"><div class="label">Turn Error</div><div class="value"><span id="turnError">0.0</span>&deg;</div></div>
<div class="card servo-card"><div class="label">Servo Angle</div><div class="value"><span id="servo">90</span>&deg;</div></div>
<div class="card motor-card"><div class="label">Motor Driver</div><div class="value" id="motor">--</div></div>
<div class="card speed-card-left"><div class="label">Encoder L</div><div class="value"><span id="encLeft">0.0</span></div></div>
<div class="card speed-card-right"><div class="label">Encoder R</div><div class="value"><span id="encRight">0.0</span></div></div>
<div class="card pid-card"><div class="label">Distance PID</div><div class="value"><span id="distancePid">0.0</span></div></div>
<div class="card pid-card"><div class="label">Turn PID</div><div class="value"><span id="turnPid">0.0</span></div></div>
</div>
<div class="card speed-card"><div class="label">Manual Speed</div><div class="value"><span id="speedValue">320</span></div><input class="slider" id="speed" type="range" min="120" max="700" value="320" oninput="speedValue.textContent=this.value"></div>
<div class="control-panel">
<div class="control-title">Manual Navigation</div>
<div class="drive-pad">
<button class="btn forward" onclick="cmd('forward')"><span class="arrow">▲</span><span class="btn-text">FORWARD</span></button>
<button class="btn left" onclick="cmd('left')"><span class="arrow">◀</span><span class="btn-text">LEFT</span></button>
<button class="btn stop" onclick="cmd('stop')">STOP</button>
<button class="btn right" onclick="cmd('right')"><span class="arrow">▶</span><span class="btn-text">RIGHT</span></button>
<button class="btn backward" onclick="cmd('backward')"><span class="arrow">▼</span><span class="btn-text">BACK</span></button>
</div>
<div class="mode-row">
<button class="btn auto" onclick="cmd('auto')">AUTO DRIVE</button>
<button class="btn idle" onclick="cmd('idle')">IDLE MODE</button>
</div>
</div>
<div class="small footer-info">WiFi AP: <span id="ssid">SmartCar_PID</span> | IP: <span id="ip">192.168.4.1</span></div>
</main>
<script>
function set(id,v){const e=document.getElementById(id);if(e)e.textContent=v}
function toggleTheme(){
  const sw=document.getElementById('themeSwitch');
  const isLight=sw.checked;
  document.body.classList.toggle('light',isLight);
  set('themeText',isLight?'Light':'Dark');
  localStorage.setItem('theme',isLight?'light':'dark');
}
(function(){
  const saved=localStorage.getItem('theme');
  const isLight=saved==='light';
  document.body.classList.toggle('light',isLight);
  window.addEventListener('load',function(){
    document.getElementById('themeSwitch').checked=isLight;
    set('themeText',isLight?'Light':'Dark');
  });
})();
function cmd(c){
  const speed=Number(document.getElementById('speed').value);
  fetch('/api/control?cmd='+encodeURIComponent(c)+'&speed='+speed).then(updateStatus).catch(()=>{});
}
function updateStatus(){
  fetch('/api/status').then(r=>r.json()).then(d=>{
    set('mode',d.mode);
    set('state',d.state);
    set('distance',Number(d.frontDistance).toFixed(1));
    set('leftDistance',Number(d.leftDistance).toFixed(1));
    set('rightDistance',Number(d.rightDistance).toFixed(1));
    set('battery',Number(d.batteryPercent).toFixed(0));
    set('voltage',Number(d.batteryVoltage).toFixed(2));
    set('leftSpeed',d.leftSpeed);
    set('rightSpeed',d.rightSpeed);
    set('encLeft',Number(d.encoderLeft).toFixed(1));
    set('encRight',Number(d.encoderRight).toFixed(1));
    set('yaw',Number(d.yaw).toFixed(1));
    set('turnError',Number(d.turnError).toFixed(1));
    set('servo',d.servoAngle);
    set('motor',d.motorReady?'Ready':'Offline');
    set('distancePid',Number(d.distancePid).toFixed(1));
    set('turnPid',Number(d.turnPid).toFixed(1));
    set('ip',d.ip);
  }).catch(()=>{});
}
setInterval(updateStatus,700);
window.addEventListener('load',updateStatus);
</script>
</body>
</html>
)rawliteral";

  static const char *modeName(robot_control::Mode mode)
  {
    if (mode == robot_control::IDLE)
      return "IDLE";
    if (mode == robot_control::MANUAL)
      return "MANUAL";
    if (mode == robot_control::AUTO_AVOIDANCE)
      return "AUTO";
    return "UNKNOWN";
  }

  static const char *stateName(robot_control::AutoState state)
  {
    if (state == robot_control::DRIVE_FRONT)
      return "DRIVE";
    if (state == robot_control::PREPARE_SCAN)
      return "PREP";
    if (state == robot_control::LOOK_LEFT)
      return "LOOK L";
    if (state == robot_control::READ_LEFT)
      return "READ L";
    if (state == robot_control::RETURN_CENTER_AFTER_LEFT)
      return "CENTER";
    if (state == robot_control::LOOK_RIGHT)
      return "LOOK R";
    if (state == robot_control::READ_RIGHT)
      return "READ R";
    if (state == robot_control::RETURN_CENTER_AFTER_RIGHT)
      return "CENTER";
    if (state == robot_control::DECIDE_TURN)
      return "DECIDE";
    if (state == robot_control::TURN_RIGHT)
      return "TURN R";
    if (state == robot_control::TURN_LEFT)
      return "TURN L";
    return "UNKNOWN";
  }

  static void addHeaders()
  {
    server.sendHeader("Cache-Control", "no-store");
    server.sendHeader("Access-Control-Allow-Origin", "*");
  }

  static void handleRoot()
  {
    addHeaders();
    server.send_P(200, "text/html", dashboardPage);
  }

  static void handleStatus()
  {
    robot_control::Status status = robot_control::getStatus();
    yahboom_motor::MotorData motorData = yahboom_motor::getData();

    float encoderLeft = (motorData.speed[0] + motorData.speed[1]) * 0.5f;
    float encoderRight = (motorData.speed[2] + motorData.speed[3]) * 0.5f;

    String json = "{";
    json += "\"mode\":\"" + String(modeName(status.mode)) + "\",";
    json += "\"state\":\"" + String(stateName(status.autoState)) + "\",";
    json += "\"frontDistance\":" + String(status.frontDistance, 2) + ",";
    json += "\"leftDistance\":" + String(status.leftDistance, 2) + ",";
    json += "\"rightDistance\":" + String(status.rightDistance, 2) + ",";
    json += "\"batteryVoltage\":" + String(battery_level::getVoltage(), 2) + ",";
    json += "\"batteryPercent\":" + String(battery_level::getPercentage()) + ",";
    json += "\"leftSpeed\":" + String(status.currentLeftSpeed) + ",";
    json += "\"rightSpeed\":" + String(status.currentRightSpeed) + ",";
    json += "\"encoderLeft\":" + String(encoderLeft, 2) + ",";
    json += "\"encoderRight\":" + String(encoderRight, 2) + ",";
    json += "\"yaw\":" + String(status.yaw, 2) + ",";
    json += "\"targetYaw\":" + String(status.targetYaw, 2) + ",";
    json += "\"turnError\":" + String(status.turnError, 2) + ",";
    json += "\"servoAngle\":" + String(status.servoAngle) + ",";
    json += "\"distancePid\":" + String(status.distancePidOutput, 2) + ",";
    json += "\"turnPid\":" + String(status.turnPidOutput, 2) + ",";
    json += "\"motorReady\":" + String(yahboom_motor::isReady() ? "true" : "false") + ",";
    json += "\"ip\":\"" + ipAddress + "\"";
    json += "}";

    addHeaders();
    server.send(200, "application/json", json);
  }

  static void handleControl()
  {
    String command = server.arg("cmd");
    int speed = server.arg("speed").toInt();

    if (speed < 120)
      speed = 120;
    if (speed > 700)
      speed = 700;

    if (command == "auto")
    {
      robot_control::setMode(robot_control::AUTO_AVOIDANCE);
    }
    else if (command == "idle")
    {
      robot_control::setMode(robot_control::IDLE);
    }
    else if (command == "stop")
    {
      robot_control::stop();
    }
    else if (command == "forward")
    {
      robot_control::setManualSpeed(speed, speed);
    }
    else if (command == "backward")
    {
      robot_control::setManualSpeed(-speed, -speed);
    }
    else if (command == "left")
    {
      robot_control::setManualSpeed(speed / 3, speed);
    }
    else if (command == "right")
    {
      robot_control::setManualSpeed(speed, speed / 3);
    }

    addHeaders();
    server.send(200, "application/json", "{\"ok\":true}");
  }

  void begin()
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSsid, apPassword);

    ipAddress = WiFi.softAPIP().toString();

    server.on("/", handleRoot);
    server.on("/api/status", handleStatus);
    server.on("/api/control", handleControl);
    server.begin();

    Serial.print("Web dashboard started at ");
    Serial.println(ipAddress);
  }

  void update()
  {
    server.handleClient();
  }

  String getIp()
  {
    return ipAddress;
  }
}
