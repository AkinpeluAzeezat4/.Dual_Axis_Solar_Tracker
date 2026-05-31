#include <Arduino.h>
#include "Pins.h"
#include "mpu6050_sensor/mpu6050_sensor.h"
#include "battery_level/battery_level.h"
#include "led_indicator/led_indicator.h"
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "vibration_motor/vibration_motor.h"
#include "sd_card/sd_card.h"
#include "posture_logic/posture_logic.h"
#include "error_handling/error_handling.h"
#include "sleep_wake/sleep_wake.h"
#include "local_server/local_server.h"
#include "reset/reset.h"

static unsigned long lastPrint = 0;

void setup()
{
  Serial.begin(115200);
  delay(500);

  Pins::begin();
  battery_level::begin();
  led_indicator::begin();
  button::begin();
  vibration_motor::begin();
  buzzer::begin();
  mpu6050_sensor::begin();
  posture_logic::begin();
  sd_card::begin();
  error_handling::begin();
  sleep_wake::begin();
  local_server::begin();
  reset::begin();

  Serial.println("Smart Posture Corrector Ready");
  Serial.print("Web Dashboard IP: ");
  Serial.println(local_server::getIP());
}

void loop()
{
  button::update();
  battery_level::update();
  mpu6050_sensor::update();
  posture_logic::update();
  error_handling::update();
  vibration_motor::update();
  buzzer::update();

  led_indicator::setBatteryLevel(battery_level::getPercentage());
  led_indicator::update();

  sd_card::update();
  local_server::update();
  sleep_wake::update();
  reset::update();

  if (button::wasPressed())
  {
    posture_logic::setMuted(!posture_logic::isMuted());
    buzzer::beep(80);
    sleep_wake::resetActivity();
  }

  if (button::wasLongPressed())
  {
    posture_logic::requestCalibration();
    sleep_wake::resetActivity();
  }

  if (millis() - lastPrint >= 1000)
  {
    lastPrint = millis();

    mpu6050_sensor::SensorData data = mpu6050_sensor::getData();

    Serial.print("STATE:");
    Serial.print(posture_logic::getStateText());
    Serial.print(" | PITCH:");
    Serial.print(data.pitch, 2);
    Serial.print(" | ROLL:");
    Serial.print(data.roll, 2);
    Serial.print(" | ERROR:");
    Serial.print(posture_logic::getPitchError(), 2);
    Serial.print(" | BAT:");
    Serial.print(battery_level::getVoltage(), 2);
    Serial.print("V ");
    Serial.print(battery_level::getPercentage());
    Serial.print(" | STORAGE:");
    Serial.print(sd_card::getStorageName());
    Serial.print(" | SD:");
    Serial.print(sd_card::isSdReady() ? "OK" : "NO");
    Serial.print(" | MUTE:");
    Serial.print(posture_logic::isMuted() ? "YES" : "NO");
    Serial.print(" | IP:");
    Serial.println(local_server::getIP());
  }
}
