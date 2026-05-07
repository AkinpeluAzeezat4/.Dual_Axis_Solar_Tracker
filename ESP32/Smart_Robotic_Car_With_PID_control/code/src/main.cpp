#include <Arduino.h>
#include "Pins.h"
#include "gyroscope/gyroscope.h"
#include "ultrasonic/ultrasonic.h"
#include "servo/servo.h"
#include "battery_level/battery_level.h"
#include "yahboom_motor/yahboom_motor.h"
#include "wifi_manager/wifi_manager.h"
#include "web_dashboard/web_dashboard.h"
#include "led_indicator/led_indicator.h"
#include "robot_control/robot_control.h"
#include "sleep_wake/sleep_wake.h"
#include "error_handling/error_handling.h"
#include "reset/reset.h"

static unsigned long lastSerialPrint = 0;

void setup()
{
  Serial.begin(115200);

  Pins::begin();
  led_indicator::begin();
  battery_level::begin();
  gyroscope::begin();
  ultrasonic::begin();
  servo::begin();
  yahboom_motor::begin();
  sleep_wake::begin();
  error_handling::begin();
  reset::begin();
  wifi_manager::begin();
  web_dashboard::begin();
  robot_control::begin();

  Serial.println("Smart Robotic Car non-blocking integration started");
  Serial.print("WiFi SSID: ");
  Serial.println(wifi_manager::getSSID());
  Serial.print("Web IP: ");
  Serial.println(wifi_manager::getIP());
}

void loop()
{
  Pins::update();
  battery_level::update();
  led_indicator::setBatteryLevel(battery_level::getPercentage());
  led_indicator::update();
  gyroscope::update();
  ultrasonic::update();
  servo::update();
  yahboom_motor::update();
  robot_control::update();
  wifi_manager::update();
  web_dashboard::update();
  sleep_wake::update();
  error_handling::update();
  reset::update();

  if (millis() - lastSerialPrint >= 1000)
  {
    lastSerialPrint = millis();
    robot_control::Status status = robot_control::getStatus();
    gyroscope::Data gyro = gyroscope::getData();

    Serial.print("Distance: ");
    Serial.print(status.frontDistance, 1);
    Serial.print(" cm | L: ");
    Serial.print(status.currentLeftSpeed);
    Serial.print(" R: ");
    Serial.print(status.currentRightSpeed);
    Serial.print(" | Battery: ");
    Serial.print(battery_level::getPercentage());
    Serial.print("% ");
    Serial.print(battery_level::getVoltage(), 2);
    Serial.print("V | Yaw: ");
    Serial.print(gyro.yaw, 1);
    Serial.print(" | Motor Config: ");
    Serial.println(yahboom_motor::isConfigured() ? "YES" : "NO");
  }
}
