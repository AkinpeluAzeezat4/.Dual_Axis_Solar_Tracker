#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "lcd_screen.h"
#include "Pins.h"
#include "temp_hum/temp_hum.h"
#include "ultrasonic/ultrasonic.h"
#include "soil_sensor/soil_sensor.h"
#include "battery_level/battery_level.h"
#include "solar_level/solar_level.h"
#include "pump/pump.h"

namespace lcd_screen
{
  static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
  static unsigned long lastUpdate = 0;
  static const unsigned long interval = 500;

  void begin()
  {
    Wire.begin(Pins::OLED_SDA, Pins::OLED_SCL);
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(10, 25, "Solar Irrigation");
    u8g2.drawStr(25, 40, "System Ready");
    u8g2.sendBuffer();
  }

  void update()
  {
    if (millis() - lastUpdate < interval)
      return;
    lastUpdate = millis();

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tf);

    char buffer[32];

    snprintf(buffer, sizeof(buffer), "Tank: %d%%", ultrasonic::getLevelPercent());
    u8g2.drawStr(0, 10, buffer);

    snprintf(buffer, sizeof(buffer), "Soil: %d%%", soil_sensor::getMoisturePercent());
    u8g2.drawStr(0, 22, buffer);

    snprintf(buffer, sizeof(buffer), "Temp: %.1fC Hum: %.0f%%", temp_hum::getTemperature(), temp_hum::getHumidity());
    u8g2.drawStr(0, 34, buffer);

    snprintf(buffer, sizeof(buffer), "Bat: %.2fV Sol: %.2fV", battery_level::getVoltage(), solar_level::getVoltage());
    u8g2.drawStr(0, 46, buffer);

    snprintf(buffer, sizeof(buffer), "P1:%s P2:%s",
             pump::isTankPumpOn() ? "ON" : "OFF",
             pump::isIrrigationPumpOn() ? "ON" : "OFF");
    u8g2.drawStr(0, 58, buffer);

    u8g2.sendBuffer();
  }
}
