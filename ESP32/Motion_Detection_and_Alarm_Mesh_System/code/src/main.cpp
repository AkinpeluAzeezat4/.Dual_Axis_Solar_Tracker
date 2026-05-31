/*SLAVE CODE*/
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
//   buzzer::update();
//   battery_level::update();
//   led_indicator::update();
//   mesh_comm::update();

//   showBatteryLevel(battery_level::getVoltage());

//   bool alarmTrigger = mesh_comm::isAlarmTriggered() || button::isTrigger();

//   if (alarmTrigger)
//   {
//     buzzer::startAlarm();
//     Serial.println("Slave alarm retriggered");
//   }
// }

/*MASTER CODE*/
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

static unsigned long lastSendTime = 0;
const unsigned long sendInterval = 500;

const uint8_t slaveMacs[3][6] = {
    {0x1C, 0x69, 0x20, 0xAE, 0xD1, 0x64},
    {0x1C, 0x69, 0x20, 0xAF, 0xAF, 0xA8},
    {0x1C, 0x69, 0x20, 0xAF, 0x04, 0xB4}};

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
  buzzer::update();
  battery_level::update();
  led_indicator::update();
  mesh_comm::update();

  showBatteryLevel(battery_level::getVoltage());

  bool alarmTrigger = pir_sensor::isMotionDetected() || button::isTrigger();

  if (alarmTrigger)
  {
    buzzer::startAlarm();

    if (millis() - lastSendTime >= sendInterval)
    {
      lastSendTime = millis();
      mesh_comm::sendAlarmTrigger();
      Serial.println("Master trigger sent to slaves");
    }
  }
}
