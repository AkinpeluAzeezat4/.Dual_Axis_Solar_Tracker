#include <Arduino.h>
#include "Pins.h"
#include "mpu6050_sensor.h"
#include "battery_level.h"
#include "led_indicator.h"
#include "button.h"
#include "buzzer.h"
#include "vibration_motor.h"
#include "sd_card.h"
#include "posture_logic.h"
#include "error_handling/error_handling.h"
#include "sleep_wake/sleep_wake.h"
#include "reset.h"

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
  reset::begin();

  Serial.println("Smart Posture Corrector Ready");
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
    Serial.print("% | SD:");
    Serial.print(sd_card::isReady() ? "OK" : "NO");
    Serial.print(" | MUTE:");
    Serial.print(posture_logic::isMuted() ? "YES" : "NO");
    Serial.print(" | ERROR_STATUS:");
    Serial.println(error_handling::getErrorText());
  }
}
