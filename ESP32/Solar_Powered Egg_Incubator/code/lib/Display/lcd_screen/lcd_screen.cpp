#include <Arduino.h>
#include <U8g2lib.h>
#include "Pins.h"
#include "lcd_screen.h"
#include "battery_level.h"
#include "solar_level.h"
#include "temp_hum.h"
#include "ultrasonic.h"
#include "rotary_encoder.h"
#include "heater.h"
#include "spinner.h"
#include "humidifier.h"
#include "rtc_clock.h"

namespace lcd_screen
{
  static U8G2_ST7920_128X64_F_SW_SPI u8g2(
      U8G2_R0,
      Pins::LCD_CLK,
      Pins::LCD_MOSI,
      Pins::LCD_CS,
      U8X8_PIN_NONE);

  static bool awake = true;
  static unsigned long lastDraw = 0;
  static unsigned long lastUserActivity = 0;

  static void drawBattery(uint8_t x, uint8_t y, uint8_t percent)
  {
    if (percent > 100)
    {
      percent = 100;
    }

    u8g2.drawFrame(x, y, 18, 7);
    u8g2.drawBox(x + 18, y + 2, 2, 3);

    uint8_t fill = (percent * 16) / 100;
    u8g2.drawBox(x + 1, y + 1, fill, 5);
  }

  void begin()
  {
    u8g2.begin();
    u8g2.setContrast(180);
    awake = true;
    lastDraw = 0;
    lastUserActivity = millis();
  }

  void update()
  {
    if (rotary_encoder::lastActivity() != 0 && rotary_encoder::lastActivity() != lastUserActivity)
    {
      lastUserActivity = rotary_encoder::lastActivity();
      wake();
    }

    if (awake && millis() - lastUserActivity >= 30000)
    {
      sleep();
    }

    if (!awake)
    {
      return;
    }

    if (millis() - lastDraw < 1000)
    {
      return;
    }

    lastDraw = millis();

    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 8, "Solar Egg Incubator");

    drawBattery(106, 1, battery_level::getPercentage());

    u8g2.setCursor(0, 20);
    u8g2.print("T:");
    u8g2.print(temp_hum::getTemperature(), 1);
    u8g2.print("C H:");
    u8g2.print(temp_hum::getHumidity(), 0);
    u8g2.print("%");

    u8g2.setCursor(0, 31);
    u8g2.print("Batt:");
    u8g2.print(battery_level::getVoltage(), 2);
    u8g2.print("V Sol:");
    u8g2.print(solar_level::getVoltage(), 1);
    u8g2.print("V");

    u8g2.setCursor(0, 42);
    u8g2.print("Dist:");
    u8g2.print(ultrasonic::getDistanceCm(), 1);
    u8g2.print("cm");

    u8g2.setCursor(0, 53);
    u8g2.print("H:");
    u8g2.print(heater::isOn() ? "ON " : "OFF");
    u8g2.print(" S:");
    u8g2.print(spinner::isOn() ? "ON " : "OFF");
    u8g2.print(" Hu:");
    u8g2.print(humidifier::isOn() ? "ON" : "OFF");

    u8g2.setCursor(0, 63);
    u8g2.print(rtc_clock::getDateTime());

    u8g2.sendBuffer();
  }

  void setBacklight(bool state)
  {
    if (state)
    {
      wake();
    }
    else
    {
      sleep();
    }
  }

  bool isAwake()
  {
    return awake;
  }

  void wake()
  {
    if (awake)
    {
      lastUserActivity = millis();
      return;
    }

    awake = true;
    lastUserActivity = millis();
    lastDraw = 0;
    u8g2.setPowerSave(0);
  }

  void sleep()
  {
    if (!awake)
    {
      return;
    }

    awake = false;
    u8g2.clearBuffer();
    u8g2.sendBuffer();
    u8g2.setPowerSave(1);
  }
}
