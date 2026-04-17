/*
// LOAD THIS FOR THE MASTER
#include <Arduino.h>
// #include "Pins/Pins.h"
#include "pir_sensor/pir_sensor.h"
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "battery_level/battery_level.h"
#include "mesh_comm/mesh_comm.h"
#include "led_indicator/led_indicator.h"
// #include "sleep_wake/sleep_wake.h"
// #include "error_handling/error_handling.h"
// #include "reset/reset.h"

#define POWER_LED_PIN 2
#define BUTTON_PIN 27
#define BUZZER_PIN 23
#define BATTERY_PIN 34
#define LED1_PIN 16
#define LED2_PIN 17
#define LED3_PIN 18
#define LED4_PIN 19
#define PIR_PIN 26

void showBatteryLevel(float voltage)
{
  if (voltage < 3.30f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.55f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.80f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, LOW);
  }
  else
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
}

void setup()
{
  // Serial.begin(115200);
  // Pins::begin();
  pir_sensor::begin(PIR_PIN);
  button::begin(BUTTON_PIN);
  buzzer::begin(BUZZER_PIN);
  battery_level::begin(BATTERY_PIN, 1.61f);
  mesh_comm::beginAsMaster();
  led_indicator::begin(POWER_LED_PIN);
  // sleep_wake::begin();
  // error_handling::begin();
  // reset::begin();

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);

  analogReadResolution(12);
  analogSetPinAttenuation(BATTERY_PIN, ADC_11db);
}

void loop()
{
  // Pins::update();
  pir_sensor::update();
  button::update();
  buzzer::update();
  battery_level::update();
  // led_indicator::update();
  mesh_comm::update();

  // sleep_wake::update();
  // error_handling::update();
  // reset::update();

  showBatteryLevel(battery_level::getVoltage());

  if (pir_sensor::isMotionDetected() || button::isTrigger())
  {
    // buzzer::startAlarm();
    led_indicator::update();
    mesh_comm::sendAlarmTrigger();
  }
}
*/

/*
 //  LOAD THIS FOR THE SLAVE

#include <Arduino.h>
#include "Pins.h"
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "battery_level/battery_level.h"
#include "led_indicator/led_indicator.h"
#include "mesh_comm/mesh_comm.h"

#define BUTTON_PIN 27
#define BATTERY_PIN 34
#define LED1_PIN 16
#define LED2_PIN 17
#define LED3_PIN 18
#define LED4_PIN 19

void showBatteryLevel(float voltage)
{
  if (voltage < 3.30f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.55f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.80f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, LOW);
  }
  else
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
}

void setup()
{
  Serial.begin(115200);

  button::begin(BUTTON_PIN);
  buzzer::begin(23);
  battery_level::begin(BATTERY_PIN, 1.61f);
  led_indicator::begin(2);
  mesh_comm::beginAsSlave();

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
}

void loop()
{
  button::update();
  buzzer::update();
  battery_level::update();
  led_indicator::update();
  mesh_comm::update();

  showBatteryLevel(battery_level::getVoltage());

  if (mesh_comm::isAlarmTriggered() || button::isTrigger())
  {
    // buzzer::startAlarm();
    Serial.println("Slave alarm triggered");
  }
}
*/

/*
#include <Arduino.h>
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "mesh_comm/mesh_comm.h"

#define BUTTON_PIN 27

uint8_t slaveMac[] = {0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC};

void setup()
{
  Serial.begin(115200);
  button::begin(BUTTON_PIN);
  buzzer::begin(23);
  mesh_comm::beginAsMaster(slaveMac, 1);
}

void loop()
{
  button::update();
  // buzzer::update();
  mesh_comm::update();

  if (button::isTrigger())
  {
    Serial.println("Master trigger sent");
    // buzzer::startAlarm();
    mesh_comm::sendAlarmTrigger();
  }
}*/

/*
#include <Arduino.h>
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "mesh_comm/mesh_comm.h"

#define BUTTON_PIN 27

void setup()
{
  Serial.begin(115200);
  button::begin(BUTTON_PIN);
  buzzer::begin(23);
  mesh_comm::beginAsSlave(1);
}

void loop()
{
  button::update();
  // buzzer::update();
  mesh_comm::update();

  if (mesh_comm::isAlarmTriggered())
  {
    Serial.println("Slave alarm triggered");
    // buzzer::startAlarm();
  }

  if (button::isTrigger())
  {
    Serial.println("Local slave test trigger");
    // buzzer::startAlarm();
  }
}

*/

/*
#include <Arduino.h>
#include "pir_sensor/pir_sensor.h"
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "battery_level/battery_level.h"
#include "led_indicator/led_indicator.h"
#include "mesh_comm/mesh_comm.h"

#define BUTTON_PIN 27
#define BATTERY_PIN 34
#define LED1_PIN 16
#define LED2_PIN 17
#define LED3_PIN 18
#define LED4_PIN 19
#define PIR_PIN 26
#define BUZZER_PIN 23

uint8_t slaveMac[] = {0x1C, 0x69, 0x20, 0xAE, 0xD1, 0x64};

void showBatteryLevel(float voltage)
{
  if (voltage < 3.30f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.55f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.80f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, LOW);
  }
  else
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
}

void setup()
{
  Serial.begin(115200);

  pir_sensor::begin(PIR_PIN);
  button::begin(BUTTON_PIN);
  buzzer::begin(BUZZER_PIN);
  battery_level::begin(BATTERY_PIN, 1.61f);
  led_indicator::begin(2);
  mesh_comm::beginAsMaster(slaveMac);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
}

void loop()
{
  pir_sensor::update();
  button::update();
  // buzzer::update();
  battery_level::update();
  led_indicator::update();
  mesh_comm::update();

  showBatteryLevel(battery_level::getVoltage());

  if (pir_sensor::isMotionDetected() || button::isTrigger())
  {
    // buzzer::startAlarm();
    mesh_comm::sendAlarmTrigger();
    Serial.println("Master trigger sent");
  }
}

*/

/*
#include <Arduino.h>
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "battery_level/battery_level.h"
#include "led_indicator/led_indicator.h"
#include "mesh_comm/mesh_comm.h"

#define BUTTON_PIN 27
#define BATTERY_PIN 34
#define LED1_PIN 16
#define LED2_PIN 17
#define LED3_PIN 18
#define LED4_PIN 19
#define BUZZER_PIN 23

void showBatteryLevel(float voltage)
{
  if (voltage < 3.30f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.55f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.80f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, LOW);
  }
  else
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
}

void setup()
{
  Serial.begin(115200);

  button::begin(BUTTON_PIN);
  buzzer::begin(BUZZER_PIN);
  battery_level::begin(BATTERY_PIN, 1.61f);
  led_indicator::begin(2);
  mesh_comm::beginAsSlave();

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
}

void loop()
{
  button::update();
  // buzzer::update();
  battery_level::update();
  led_indicator::update();
  mesh_comm::update();

  showBatteryLevel(battery_level::getVoltage());

  if (mesh_comm::isAlarmTriggered() || button::isTrigger())
  {
    // buzzer::startAlarm();
    Serial.println("Slave alarm triggered");
  }
}


*/

// #include <Arduino.h>
// #include "button/button.h"
// #include "buzzer/buzzer.h"
// #include "battery_level/battery_level.h"
// #include "led_indicator/led_indicator.h"
// #include "mesh_comm/mesh_comm.h"

// #define BUTTON_PIN 27
// #define BATTERY_PIN 34
// #define LED1_PIN 16
// #define LED2_PIN 17
// #define LED3_PIN 18
// #define LED4_PIN 19
// #define BUZZER_PIN 23

// void showBatteryLevel(float voltage)
// {
//   if (voltage < 3.30f)
//   {
//     digitalWrite(LED1_PIN, HIGH);
//     digitalWrite(LED2_PIN, LOW);
//     digitalWrite(LED3_PIN, LOW);
//     digitalWrite(LED4_PIN, LOW);
//   }
//   else if (voltage < 3.55f)
//   {
//     digitalWrite(LED1_PIN, HIGH);
//     digitalWrite(LED2_PIN, HIGH);
//     digitalWrite(LED3_PIN, LOW);
//     digitalWrite(LED4_PIN, LOW);
//   }
//   else if (voltage < 3.80f)
//   {
//     digitalWrite(LED1_PIN, HIGH);
//     digitalWrite(LED2_PIN, HIGH);
//     digitalWrite(LED3_PIN, HIGH);
//     digitalWrite(LED4_PIN, LOW);
//   }
//   else
//   {
//     digitalWrite(LED1_PIN, HIGH);
//     digitalWrite(LED2_PIN, HIGH);
//     digitalWrite(LED3_PIN, HIGH);
//     digitalWrite(LED4_PIN, HIGH);
//   }
// }

// void setup()
// {
//   Serial.begin(115200);

//   button::begin(BUTTON_PIN);
//   buzzer::begin(BUZZER_PIN);
//   battery_level::begin(BATTERY_PIN, 1.61f);
//   led_indicator::begin(2);
//   mesh_comm::beginAsSlave();

//   pinMode(LED1_PIN, OUTPUT);
//   pinMode(LED2_PIN, OUTPUT);
//   pinMode(LED3_PIN, OUTPUT);
//   pinMode(LED4_PIN, OUTPUT);
// }

// void loop()
// {
//   button::update();
//   // buzzer::update();
//   battery_level::update();
//   led_indicator::update();
//   mesh_comm::update();

//   showBatteryLevel(battery_level::getVoltage());

//   if (mesh_comm::isAlarmTriggered() || button::isTrigger())
//   {
//     // buzzer::startAlarm();
//     Serial.println("Slave alarm triggered");
//   }
// }

#include <Arduino.h>
#include "pir_sensor/pir_sensor.h"
#include "button/button.h"
#include "buzzer/buzzer.h"
#include "battery_level/battery_level.h"
#include "led_indicator/led_indicator.h"
#include "mesh_comm/mesh_comm.h"

#define BUTTON_PIN 27
#define BATTERY_PIN 34
#define LED1_PIN 16
#define LED2_PIN 17
#define LED3_PIN 18
#define LED4_PIN 19
#define PIR_PIN 26
#define BUZZER_PIN 23

const uint8_t slaveMacs[3][6] = {
    {0x1C, 0x69, 0x20, 0xAE, 0xD1, 0x64},
    {0x1C, 0x69, 0x20, 0xAF, 0xAF, 0xA8},
    {0x30, 0xC6, 0xF7, 0x11, 0x22, 0x33}};

void showBatteryLevel(float voltage)
{
  if (voltage < 3.30f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.55f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, LOW);
    digitalWrite(LED4_PIN, LOW);
  }
  else if (voltage < 3.80f)
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, LOW);
  }
  else
  {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    digitalWrite(LED3_PIN, HIGH);
    digitalWrite(LED4_PIN, HIGH);
  }
}

void setup()
{
  Serial.begin(115200);

  pir_sensor::begin(PIR_PIN);
  button::begin(BUTTON_PIN);
  buzzer::begin(BUZZER_PIN);
  battery_level::begin(BATTERY_PIN, 1.61f);
  led_indicator::begin(2);
  mesh_comm::beginAsMaster(slaveMacs, 3);

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  pinMode(LED4_PIN, OUTPUT);
}

void loop()
{
  pir_sensor::update();
  button::update();
  // buzzer::update();
  battery_level::update();
  led_indicator::update();
  mesh_comm::update();

  showBatteryLevel(battery_level::getVoltage());

  if (pir_sensor::isMotionDetected() || button::isTrigger())
  {
    // buzzer::startAlarm();
    mesh_comm::sendAlarmTrigger();
    Serial.println("Master trigger sent to all slaves");
  }
}
