#include <Arduino.h>
#include "Pins.h"
#include "rotary_encoder/rotary_encoder.h"
#include "load_relay/load_relay.h"
#include "source_relay/source_relay.h"
#include "led_indicator/led_indicator.h"
#include "lcd_screen/lcd_screen.h"
#include "shared_var/shared_var.h"
#include "config_manager/config_manager.h"
#include "pzem_sensor/pzem_sensor.h"
#include "nepa_sense/nepa_sense.h"
#include "inverter_sense/inverter_sense.h"
#include "load_manager/load_manager.h"
#include "local_server/local_server.h"
#include "ndelay/ndelay.h"

unsigned long lastLoadUpdate = 0;
const unsigned long loadInterval = 3000;
bool showConsumption = false;

bool bootCheckActive = true;
unsigned long bootCheckStart = 0;
unsigned long bootButtonStart = 0;
const unsigned long bootWindow = 5000;
const unsigned long bootHoldTime = 3000;

void handleBootSetupWindow()
{
  if (!bootCheckActive) return;

  unsigned long now = millis();

  if (bootCheckStart == 0)
    bootCheckStart = now;

  if (rotary_encoder::isPressed())
  {
    if (bootButtonStart == 0)
      bootButtonStart = now;

    if (now - bootButtonStart >= bootHoldTime)
    {
      shared_var::settingsMode = true;
      shared_var::waitRelease = true;
      bootCheckActive = false;
      rotary_encoder::lockUntilRelease();
    }
  }
  else
  {
    bootButtonStart = 0;
  }

  if (now - bootCheckStart >= bootWindow)
    bootCheckActive = false;
}

void handleRuntimeSettingsEntry()
{
  if (!shared_var::settingsMode && rotary_encoder::isHeld(5000))
  {
    shared_var::settingsMode = true;
    shared_var::waitRelease = true;
    rotary_encoder::lockUntilRelease();
  }

  if (shared_var::waitRelease)
  {
    if (!rotary_encoder::isPressed())
      shared_var::waitRelease = false;
  }
}

void updateLcd()
{
  unsigned long now = millis();

  if (shared_var::settingsMode)
  {
    if (!shared_var::waitRelease)
      lcd_screen::update("settings");
    return;
  }

  if (now - lastLoadUpdate >= loadInterval)
  {
    lastLoadUpdate = now;

    if (showConsumption)
      lcd_screen::update("loadConsumption");
    else
      lcd_screen::update("loadStatus");

    showConsumption = !showConsumption;
  }
}

void setup()
{
  Serial.begin(115200);
  Pins::begin();
  config_manager::begin();
  rotary_encoder::begin();
  load_relay::begin();
  source_relay::begin();
  pzem_sensor::begin();
  nepa_sense::begin();
  inverter_sense::begin();
  load_manager::begin();
  lcd_screen::begin();
  led_indicator::begin(Pins::HEARTBEAT_LED);
  ndelay::begin();
  local_server::begin();
}

void loop()
{
  rotary_encoder::update();
  led_indicator::update();
  ndelay::update();
  pzem_sensor::update();
  nepa_sense::update();
  inverter_sense::update();
  source_relay::update();
  local_server::update();

  handleBootSetupWindow();
  handleRuntimeSettingsEntry();
  updateLcd();

  if (!shared_var::settingsMode)
    load_manager::update();

  load_relay::update();
}
