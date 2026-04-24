#include <Arduino.h>

// // #include "Pins/Pins.h"
// // #include "NFC_module/NFC_module.h"
// // #include "button/button.h"
// // #include "buzzer/buzzer.h"
#include "battery_level/battery_level.h"

#include "WiFi/WiFi.h"
#include "oled_screen/oled_screen.h"
#include "led_indicator/led_indicator.h"
// // #include "sd_card/sd_card.h"
// // #include "sleep_wake/sleep_wake.h"
// // #include "error_handling/error_handling.h"
// // #include "rtc/rtc.h"
// // #include "reset/reset.h"

#include <Wire.h>
#define LED_PIN 48
#define BATTERY_PIN 1
#define OLED_SDA_PIN 8
#define OLED_SCL_PIN 9

unsigned long lastSwitch = 0;
uint8_t screenState = 0;

void setup()
{
  // Pins::begin();
  // NFC_module::begin();
  // button::begin();
  // buzzer::begin();
  battery_level::begin(BATTERY_PIN);

  WiFi::begin();

  oled_screen::begin(OLED_SDA_PIN, OLED_SCL_PIN);
  led_indicator::begin(LED_PIN);
  // sd_card::begin();
  // sleep_wake::begin();
  // error_handling::begin();
  // rtc::begin();
  // reset::begin();
  // oled_screen::update("welcome");
}

void loop()
{
  // Pins::update();
  // NFC_module::update();
  // button::update();
  // buzzer::update();
  battery_level::update();

  WiFi::update();
  // oled_screen::update();
  // led_indicator::update();
  // sd_card::update();
  // sleep_wake::update();
  // error_handling::update();
  // rtc::update();
  // reset::update();

  oled_screen::update("test");
  led_indicator::update("normal");
}
