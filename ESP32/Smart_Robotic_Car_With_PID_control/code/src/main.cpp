#include <Arduino.h>
#include "Pins.h"
#include "gyroscope/gyroscope.h"
#include "ultrasonic/ultrasonic.h"
#include "servo/servo.h"
#include "battery_level/battery_level.h"
#include "yahboom_motor/yahboom_motor.h"
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
  web_dashboard::begin();
  robot_control::begin();
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
  web_dashboard::update();
  sleep_wake::update();
  error_handling::update();
  reset::update();

  if (millis() - lastSerialPrint >= 1000)
  {
    lastSerialPrint = millis();
    robot_control::Status status = robot_control::getStatus();
    gyroscope::Data gyro = gyroscope::getData();
  }
}
