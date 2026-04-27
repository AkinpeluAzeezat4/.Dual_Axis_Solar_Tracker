#include <Arduino.h>
// #include "Pins.h"
// #include "pzem_sensor/pzem_sensor.h"
#include "rotary_encoder/rotary_encoder.h"
#include "load_relay/load_relay.h"
#include "led_indicator/led_indicator.h"
#include "ndelay/ndelay.h"
#include "shared_var/shared_var.h"
// #include "rotary_encoder/rotary_encoder.h"
// #include "source_relay/source_relay.h"
// #include "gpio_expander/gpio_expander.h"
// #include "regulator/regulator.h"
// #include "nepa_sense/nepa_sense.h"
// #include "inverter_sense/inverter_sense.h"

// #include "WiFi/WiFi.h"
#include "lcd_screen/lcd_screen.h"
// #include "led_indicator/led_indicator.h"
// #include "sleep_wake/sleep_wake.h"
// #include "error_handling/error_handling.h"
// #include "reset/reset.h"

unsigned long encoderPressStart = 0;
bool encoderHeld = false;

bool showConsumption = false;

unsigned long lastLoadUpdate = 0;
const unsigned long loadInterval = 3000;

bool temp = false;
// int led = 2;

static bool holdDetected = false;
void setup()
{
  Serial.begin(115200);
  // pinMode(led, OUTPUT);
  //   Pins::begin();
  //   pzem_sensor::begin();

  rotary_encoder::begin();
  load_relay::begin();
  ndelay::begin();
  //   source_relay::begin();
  //   gpio_expander::begin();
  //   regulator::begin();
  //   nepa_sense::begin();
  //   inverter_sense::begin();

  //   WiFi::begin();
  lcd_screen::begin();
  led_indicator::begin(2);
  //   led_indicator::begin();
  //   sleep_wake::begin();
  //   error_handling::begin();
  //   reset::begin();
}

void loop()
{
  unsigned long now = millis();
  //   Pins::update();
  //   pzem_sensor::update();
  rotary_encoder::update();
  led_indicator::update();
  ndelay::update();
  load_relay::update();
  //   source_relay::update();
  //   gpio_expander::update();
  //   regulator::update();
  //   nepa_sense::update();
  //   inverter_sense::update();

  //   WiFi::update();
  // lcd_screen::update("settings");
  //   led_indicator::update();
  //   sleep_wake::update();
  //   error_handling::update();
  //   reset::update();

  // if (buttonState == LOW && lastButtonState == HIGH)
  // {
  //   Serial.println("Button Pressed");
  //   delay(200); // simple debounce
  // }
  // lastButtonState = buttonState;

  // delay(1); // tiny delay to stabilize readings

  // if (rotary_encoder::wasPressed())
  // {
  //   int sel = lcd_screen::getSelectedItem();
  //   switch (sel)
  //   {
  //   case 0:
  //     lcd_screen::relayPowerSetup();
  //     if (lcd_screen::isRelaySetupFinished())
  //     {
  //       int r1 = lcd_screen::getRelayPower(0);
  //       int r2 = lcd_screen::getRelayPower(1);
  //       int r3 = lcd_screen::getRelayPower(2);
  //       int r4 = lcd_screen::getRelayPower(3);
  //       int r5 = lcd_screen::getRelayPower(4);
  //       int r6 = lcd_screen::getRelayPower(5);

  //       lcd_screen::resetRelaySetup();
  //     }
  //     break;
  //   case 1:
  //     // Serial.println("Timer Setup");
  //     break;
  //   case 2:
  //     // Serial.println("WiFi Config");
  //     break;
  //   case 3:
  //     // Serial.println("System Info");
  //     break;
  //   }
  //   delay(250); // debounce
  // }

  if (rotary_encoder::isHeld(5000))
  {
    shared_var::settingsMode = true;
  }

  // ---------- LCD UPDATE ----------
  if (shared_var::settingsMode)
  {
    lcd_screen::update("settings");
    return;
  }

  else if (now - lastLoadUpdate >= loadInterval)
  {
    lastLoadUpdate = now;

    if (showConsumption)
      lcd_screen::update("loadConsumption");
    else
      lcd_screen::update("loadStatus");
    showConsumption = !showConsumption;
  }

  // ---------- Relay Priority Logic ----------
  if (lcd_screen::getSystemPower() > lcd_screen::getInverterPower())
  {
    int power[6];
    for (int i = 0; i < 6; i++)
      power[i] = lcd_screen::getRelayPower(i);

    int priority[6] = {0, 1, 2, 3, 4, 5};

    // sort relays by ascending power (lowest first)
    for (int i = 0; i < 5; i++)
    {
      for (int j = i + 1; j < 6; j++)
      {
        if (power[priority[i]] > power[priority[j]])
        {
          int temp = priority[i];
          priority[i] = priority[j];
          priority[j] = temp;
        }
      }
    }

    int allowedPower = 0;

    for (int i = 0; i < 6; i++)
    {
      int relay = priority[i];
      int p = power[relay];

      if (allowedPower + p <= lcd_screen::getInverterPower())
      {
        load_relay::setRelay(relay, true);
        allowedPower += p;
      }
      else
      {
        load_relay::setRelay(relay, false);
      }
    }
  }
}
