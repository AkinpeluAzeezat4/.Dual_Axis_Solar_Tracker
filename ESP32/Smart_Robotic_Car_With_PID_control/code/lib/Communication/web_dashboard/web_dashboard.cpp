#include <Arduino.h>
#include <WebServer.h>
#include "web_dashboard.h"
#include "wifi_manager/wifi_manager.h"
#include "robot_control/robot_control.h"
#include "ultrasonic/ultrasonic.h"
#include "battery_level/battery_level.h"
#include "gyroscope/gyroscope.h"
#include "yahboom_motor/yahboom_motor.h"

namespace web_dashboard
{
    static WebServer server(80);

    static const char page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Robotic Car</title>
<style>
body{font-family:Arial;margin:0;background:#10131a;color:#f3f6fb}header{padding:18px;background:#1d2430}main{padding:16px;max-width:980px;margin:auto}.grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(155px,1fr));gap:12px}.card{background:#1b2230;border-radius:16px;padding:16px;box-shadow:0 4px 16px #0005}.label{font-size:13px;color:#9aa6b2}.value{font-size:27px;font-weight:700;margin-top:6px}.small{font-size:15px;color:#c7d0dc}.controls{display:grid;grid-template-columns:repeat(3,1fr);gap:10px;margin-top:16px}.btn{border:0;border-radius:14px;padding:14px;font-size:16px;font-weight:700;background:#2f6fed;color:white}.btn.stop{background:#cc3344}.btn.auto{background:#228a55}.btn.idle{background:#555}.span3{grid-column:span 3}.slider{width:100%;margin-top:10px}@media(max-width:520px){.controls{grid-template-columns:1fr}.span3{grid-column:span 1}}
</style>
</head>
<body>
<header><h2>Smart Robotic Car Dashboard</h2><div class="small">ESP32 self-driving PID control</div></header>
<main>
<div class="grid">
<div class="card"><div class="label">Mode</div><div class="value" id="mode">--</div></div>
<div class="card"><div class="label">Auto State</div><div class="value" id="state">--</div></div>
<div class="card"><div class="label">Front Distance</div><div class="value"><span id="distance">--</span> cm</div></div>
<div class="card"><div class="label">Battery</div><div class="value"><span id="battery">--</span>%</div><div class="small"><span id="voltage">--</span> V</div></div>
<div class="card"><div class="label">Left Speed</div><div class="value" id="leftSpeed">--</div></div>
<div class="card"><div class="label">Right Speed</div><div class="value" id="rightSpeed">--</div></div>
<div class="card"><div class="label">Right Scan</div><div class="value"><span id="rightDistance">--</span> cm</div></div>
<div class="card"><div class="label">Left Scan</div><div class="value"><span id="leftDistance">--</span> cm</div></div>
<div class="card"><div class="label">Yaw</div><div class="value"><span id="yaw">--</span>&deg;</div></div>
<div class="card"><div class="label">Pitch / Roll</div><div class="value"><span id="pitch">--</span> / <span id="roll">--</span></div></div>
<div class="card"><div class="label">Servo Angle</div><div class="value"><span id="servo">--</span>&deg;</div></div>
<div class="card"><div class="label">Motor Driver</div><div class="value" id="motor">--</div></div>
</div>
<div class="card" style="margin-top:14px"><div class="label">Manual Speed</div><div class="value"><span id="speedValue">320</span></div><input class="slider" id="speed" type="range" min="120" max="700" value="320" oninput="speedValue.textContent=this.value"></div>
<div class="controls">
<button class="btn auto span3" onclick="cmd('auto')">AUTO DRIVE</button>
<button class="btn" onclick="cmd('left')">LEFT</button>
<button class="btn" onclick="cmd('forward')">FORWARD</button>
<button class="btn" onclick="cmd('right')">RIGHT</button>
<button class="btn" onclick="cmd('backward')">BACKWARD</button>
<button class="btn stop" onclick="cmd('stop')">STOP</button>
<button class="btn idle" onclick="cmd('idle')">IDLE</button>
</div>
<div class="small" style="margin-top:16px">WiFi AP: <span id="ssid">--</span> | IP: <span id="ip">--</span></div>
</main>
<script>
function v(){return document.getElementById('speed').value}
function cmd(c){fetch('/api/control?cmd='+c+'&speed='+v()).then(update)}
function set(id,v){document.getElementById(id).textContent=v}
function update(){fetch('/api/status').then(r=>r.json()).then(d=>{
set('mode',d.mode);set('state',d.state);set('distance',d.frontDistance.toFixed(1));set('battery',d.batteryPercent);set('voltage',d.batteryVoltage.toFixed(2));set('leftSpeed',d.currentLeftSpeed);set('rightSpeed',d.currentRightSpeed);set('rightDistance',d.rightDistance.toFixed(1));set('leftDistance',d.leftDistance.toFixed(1));set('yaw',d.yaw.toFixed(1));set('pitch',d.pitch.toFixed(1));set('roll',d.roll.toFixed(1));set('servo',d.servoAngle);set('motor',d.motorConfigured?'Ready':'Config');set('ip',d.ip);set('ssid',d.ssid);
}).catch(e=>{})}
setInterval(update,500);update();
</script>
</body>
</html>
)rawliteral";

    static const char *modeName(robot_control::Mode mode)
    {
        if (mode == robot_control::IDLE) return "IDLE";
        if (mode == robot_control::MANUAL) return "MANUAL";
        return "AUTO";
    }

    static const char *stateName(robot_control::AutoState state)
    {
        if (state == robot_control::DRIVE_FORWARD) return "DRIVE";
        if (state == robot_control::STOP_FOR_SCAN) return "STOP";
        if (state == robot_control::SCAN_RIGHT) return "SCAN R";
        if (state == robot_control::SCAN_LEFT) return "SCAN L";
        if (state == robot_control::DECIDE_DIRECTION) return "DECIDE";
        if (state == robot_control::TURN_RIGHT) return "TURN R";
        if (state == robot_control::TURN_LEFT) return "TURN L";
        if (state == robot_control::REVERSE_BACK) return "REVERSE";
        return "UNKNOWN";
    }

    static int16_t controlSpeed()
    {
        if (!server.hasArg("speed")) return 320;
        return constrain(server.arg("speed").toInt(), 120, 700);
    }

    static void handleRoot()
    {
        server.send(200, "text/html", page);
    }

    static void handleStatus()
    {
        robot_control::Status status = robot_control::getStatus();
        gyroscope::Data gyro = gyroscope::getData();
        yahboom_motor::MotorData motor = yahboom_motor::getData();

        String json = "{";
        json += "\"mode\":\"" + String(modeName(status.mode)) + "\",";
        json += "\"state\":\"" + String(stateName(status.autoState)) + "\",";
        json += "\"frontDistance\":" + String(status.frontDistance, 2) + ",";
        json += "\"rightDistance\":" + String(status.rightDistance, 2) + ",";
        json += "\"leftDistance\":" + String(status.leftDistance, 2) + ",";
        json += "\"batteryVoltage\":" + String(battery_level::getVoltage(), 2) + ",";
        json += "\"batteryPercent\":" + String(battery_level::getPercentage()) + ",";
        json += "\"targetLeftSpeed\":" + String(status.targetLeftSpeed) + ",";
        json += "\"targetRightSpeed\":" + String(status.targetRightSpeed) + ",";
        json += "\"currentLeftSpeed\":" + String(status.currentLeftSpeed) + ",";
        json += "\"currentRightSpeed\":" + String(status.currentRightSpeed) + ",";
        json += "\"measuredM1\":" + String(motor.measuredSpeed[0], 2) + ",";
        json += "\"measuredM2\":" + String(motor.measuredSpeed[1], 2) + ",";
        json += "\"measuredM3\":" + String(motor.measuredSpeed[2], 2) + ",";
        json += "\"measuredM4\":" + String(motor.measuredSpeed[3], 2) + ",";
        json += "\"yaw\":" + String(gyro.yaw, 2) + ",";
        json += "\"pitch\":" + String(gyro.pitch, 2) + ",";
        json += "\"roll\":" + String(gyro.roll, 2) + ",";
        json += "\"servoAngle\":" + String(status.servoAngle) + ",";
        json += "\"motorConfigured\":" + String(yahboom_motor::isConfigured() ? "true" : "false") + ",";
        json += "\"ssid\":\"" + String(wifi_manager::getSSID()) + "\",";
        json += "\"ip\":\"" + wifi_manager::getIP().toString() + "\"";
        json += "}";

        server.send(200, "application/json", json);
    }

    static void handleControl()
    {
        String command = server.arg("cmd");
        int16_t speed = controlSpeed();

        if (command == "auto") robot_control::setMode(robot_control::AUTO_DRIVE);
        else if (command == "idle") robot_control::setMode(robot_control::IDLE);
        else if (command == "stop") robot_control::stop();
        else if (command == "forward") robot_control::setManualSpeed(speed, speed);
        else if (command == "backward") robot_control::setManualSpeed(-speed, -speed);
        else if (command == "left") robot_control::setManualSpeed(-speed, speed);
        else if (command == "right") robot_control::setManualSpeed(speed, -speed);
        else if (command == "manual" && server.hasArg("left") && server.hasArg("right")) robot_control::setManualSpeed(server.arg("left").toInt(), server.arg("right").toInt());

        server.send(200, "application/json", "{\"ok\":true}");
    }

    void begin()
    {
        server.on("/", handleRoot);
        server.on("/api/status", handleStatus);
        server.on("/api/control", handleControl);
        server.begin();
    }

    void update()
    {
        server.handleClient();
    }
}
