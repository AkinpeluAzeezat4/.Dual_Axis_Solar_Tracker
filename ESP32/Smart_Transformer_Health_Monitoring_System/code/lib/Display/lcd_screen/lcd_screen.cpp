#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Pins.h"
#include "lcd_screen.h"
#include "temp_sensor/temp_sensor.h"
#include "current_sensor/current_sensor.h"
#include "vibration_sensor/vibration_sensor.h"
#include "load_relay/load_relay.h"
#include "sd_card/sd_card.h"
#include "rotary_encoder/rotary_encoder.h"

namespace lcd_screen
{
  static LiquidCrystal_I2C lcd(0x27, 20, 4);

  static unsigned long lastUpdate = 0;
  static uint8_t screen = 0;
  static const uint8_t screenCount = 3;

  static void printFixed(uint8_t col, uint8_t row, const String &text)
  {
    lcd.setCursor(col, row);

    String out = text;

    while (out.length() < 20 - col)
    {
      out += ' ';
    }

    if (out.length() > 20 - col)
    {
      out = out.substring(0, 20 - col);
    }

    lcd.print(out);
  }

  static void drawScreen()
  {
    if (screen == 0)
    {
      printFixed(0, 0, "MOTOR LIVE DATA");
      printFixed(0, 1, "Temp: " + String(temp_sensor::getTemperatureC(), 1) + " C");
      printFixed(0, 2, "Curr: " + String(current_sensor::getCurrentA(), 2) + " A");
      printFixed(0, 3, "Vib : " + String(vibration_sensor::getVibrationRMS(), 3) + " g");
    }
    else if (screen == 1)
    {
      printFixed(0, 0, "SYSTEM STATUS");
      printFixed(0, 1, String("Relay: ") + (load_relay::isOn() ? "ON" : "OFF"));
      printFixed(0, 2, String("Fault: ") + (load_relay::isFault() ? "YES" : "NO"));
      printFixed(0, 3, String("SD: ") + (sd_card::isReady() ? "READY" : "NOT READY"));
    }
    else
    {
      printFixed(0, 0, "ACCELERATION g");
      printFixed(0, 1, "X: " + String(vibration_sensor::getX(), 2));
      printFixed(0, 2, "Y: " + String(vibration_sensor::getY(), 2));
      printFixed(0, 3, "Z: " + String(vibration_sensor::getZ(), 2));
    }
  }

  void begin()
  {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);

    lcd.init();
    lcd.backlight();
    lcd.clear();

    printFixed(0, 0, "Predictive Maint.");
    printFixed(0, 1, "ESP32-S3 Starting");
    printFixed(0, 2, "LCD + ADXL345 I2C");
    printFixed(0, 3, "Please wait...");
  }

  void update()
  {
    bool screenChanged = false;

    int delta = rotary_encoder::getDelta();

    if (delta > 0)
    {
      screen = (screen + 1) % screenCount;
      screenChanged = true;
    }
    else if (delta < 0)
    {
      screen = (screen + screenCount - 1) % screenCount;
      screenChanged = true;
    }

    if (!screenChanged && millis() - lastUpdate < 500)
    {
      return;
    }

    lastUpdate = millis();
    drawScreen();
  }

  void setScreen(uint8_t index)
  {
    if (index >= screenCount)
    {
      return;
    }

    screen = index;
    lastUpdate = 0;
  }

  uint8_t getScreen()
  {
    return screen;
  }
}
