#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "lcd_screen.h"
#include "Pins.h"
#include "temp_hum/temp_hum.h"
#include "ultrasonic/ultrasonic.h"
#include "soil_sensor/soil_sensor.h"
#include "battery_level/battery_level.h"
#include "solar_level/solar_level.h"
#include "pump/pump.h"
#include "system_logic/system_logic.h"

namespace lcd_screen
{
  static LiquidCrystal_I2C lcd(0x27, 20, 4);
  static unsigned long lastUpdate = 0;
  static const unsigned long interval = 1000;
  static bool autoRefresh = true;

  static void printLine(uint8_t row, const String &text)
  {
    String line = text;

    if (line.length() > 20)
    {
      line = line.substring(0, 20);
    }

    while (line.length() < 20)
    {
      line += " ";
    }

    lcd.setCursor(0, row);
    lcd.print(line);
  }

  void begin()
  {
    Wire.begin(Pins::OLED_SDA, Pins::OLED_SCL);

    lcd.init();
    lcd.backlight();
    lcd.clear();

    printLine(0, "Solar Irrigation");
    printLine(1, "Autonomous System");
    printLine(2, "Hold Encoder 5s");
    printLine(3, "For Setup Mode");
  }

  void update()
  {
    if (!autoRefresh)
    {
      return;
    }

    if (millis() - lastUpdate < interval)
    {
      return;
    }

    lastUpdate = millis();

    char line[24];

    snprintf(line, sizeof(line), "Tank:%3u%% Soil:%3u%%", ultrasonic::getLevelPercent(), soil_sensor::getMoisturePercent());
    printLine(0, line);

    snprintf(line, sizeof(line), "Bat:%3u%% %.2fV", battery_level::getPercentage(), battery_level::getVoltage());
    printLine(1, line);

    snprintf(line, sizeof(line), "TP:%s IP:%s Sol:%.1f", pump::isTankPumpOn() ? "ON " : "OFF", pump::isIrrigationPumpOn() ? "ON " : "OFF", solar_level::getVoltage());
    printLine(2, line);

    printLine(3, system_logic::getStatusText());
  }

  void showMessage(const char *line1, const char *line2, const char *line3, const char *line4)
  {
    printLine(0, line1);
    printLine(1, line2);
    printLine(2, line3);
    printLine(3, line4);
  }

  void setAutoRefresh(bool state)
  {
    autoRefresh = state;
    lastUpdate = 0;
  }

  bool isAutoRefreshEnabled()
  {
    return autoRefresh;
  }
}
