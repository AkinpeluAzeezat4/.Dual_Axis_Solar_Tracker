#include <Arduino.h>
#include "Pins.h"
#include "sun_sensor/sun_sensor.h"
#include "rotary_encoder/rotary_encoder.h"
#include "motor_driver/motor_driver.h"
#include "led_indicator/led_indicator.h"
#include "sleep_wake/sleep_wake.h"
#include "error_handling/error_handling.h"
// #include "reset/reset.h"

unsigned long lastPrint = 0;

void setup()
{
  Serial.begin(115200);

  Pins::begin();
  sun_sensor::begin();
  rotary_encoder::begin();
  motor_driver::begin();
  led_indicator::begin(Pins::LED_STATUS);
  sleep_wake::begin();
  error_handling::begin();
  // reset::begin();
}

void loop()
{
  sun_sensor::update();
  rotary_encoder::update();
  motor_driver::update();
  led_indicator::update();
  sleep_wake::update();
  error_handling::update();
  // reset::update();

  if (millis() - lastPrint >= 1000)
  {
    lastPrint = millis();

    sun_sensor::SensorData d = sun_sensor::getData();

    Serial.print("MODE:");
    Serial.print(motor_driver::getModeName());
    Serial.print(" TL:");
    Serial.print(d.topLeft);
    Serial.print(" TR:");
    Serial.print(d.topRight);
    Serial.print(" BL:");
    Serial.print(d.bottomLeft);
    Serial.print(" BR:");
    Serial.print(d.bottomRight);
    Serial.print(" NIGHT:");
    Serial.print(d.night);
    Serial.print(" H:");
    Serial.print(d.horizontalError);
    Serial.print(" V:");
    Serial.print(d.verticalError);
    Serial.print(" DB:");
    Serial.print(sun_sensor::getDeadband());
    Serial.print(" NT:");
    Serial.println(sun_sensor::getNightThreshold());
  }
}

