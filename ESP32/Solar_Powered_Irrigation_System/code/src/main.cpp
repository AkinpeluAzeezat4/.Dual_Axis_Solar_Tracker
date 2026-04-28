#include <Arduino.h>
#include "Pins.h"
#include "settings_manager/settings_manager.h"
#include "temp_hum/temp_hum.h"
#include "ultrasonic/ultrasonic.h"
#include "soil_sensor/soil_sensor.h"
#include "button/button.h"
#include "rotary_encoder/rotary_encoder.h"
#include "buzzer/buzzer.h"
#include "pump/pump.h"
#include "battery_level/battery_level.h"
#include "solar_level/solar_level.h"
#include "lcd_screen/lcd_screen.h"
#include "led_indicator/led_indicator.h"
#include "sleep_wake/sleep_wake.h"
#include "error_handling/error_handling.h"
#include "reset/reset.h"

void setup()
{
  Serial.begin(115200);

  Pins::begin();
  settings_manager::begin();

  button::begin();
  rotary_encoder::begin();

  temp_hum::begin();
  ultrasonic::begin();
  soil_sensor::begin();
  battery_level::begin();
  solar_level::begin();

  pump::begin();
  buzzer::begin();
  led_indicator::begin();
  lcd_screen::begin();

  sleep_wake::begin();
  error_handling::begin();
  reset::begin();
}

void loop()
{
  Pins::update();

  button::update();
  rotary_encoder::update();

  temp_hum::update();
  ultrasonic::update();
  soil_sensor::update();
  battery_level::update();
  solar_level::update();

  error_handling::update();
  pump::update();

  buzzer::update();
  led_indicator::update();
  lcd_screen::update();

  sleep_wake::update();
  reset::update();
}
