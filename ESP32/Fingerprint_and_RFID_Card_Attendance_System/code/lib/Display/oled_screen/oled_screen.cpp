#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "oled_screen.h"
#include "Pins.h"

namespace oled_screen
{
  static U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

  static bool ready = false;
  static String titleText = "";
  static String lineText1 = "";
  static String lineText2 = "";
  static String lineText3 = "";
  static bool dirty = true;
  static unsigned long lastRender = 0;

  static String fitText(const String &text, uint8_t maxChars)
  {
    if (text.length() <= maxChars)
      return text;

    if (maxChars <= 2)
      return text.substring(0, maxChars);

    return text.substring(0, maxChars - 2) + "..";
  }

  static void render()
  {
    if (!ready)
      return;

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x12_tf);

    u8g2.drawStr(0, 10, fitText(titleText, 21).c_str());
    u8g2.drawHLine(0, 13, 128);

    u8g2.drawStr(0, 29, fitText(lineText1, 21).c_str());
    u8g2.drawStr(0, 43, fitText(lineText2, 21).c_str());
    u8g2.drawStr(0, 57, fitText(lineText3, 21).c_str());

    u8g2.sendBuffer();

    dirty = false;
    lastRender = millis();
  }

  void begin()
  {
    Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);

    ready = u8g2.begin();

    if (!ready)
      return;

    showBoot();
    render();
  }

  void update()
  {
    if (!ready || !dirty)
      return;

    if (millis() - lastRender < 80)
      return;

    render();
  }

  void show(const String &title, const String &line1, const String &line2, const String &line3)
  {
    titleText = title;
    lineText1 = line1;
    lineText2 = line2;
    lineText3 = line3;
    dirty = true;
  }

  void showBoot()
  {
    show("Attendance System", "Fingerprint + RFID", "Starting...", "Please wait");
  }

  void showReady(const String &ip)
  {
    show("System Ready", "Tap card first", "Then fingerprint", "Web: " + ip);
  }

  void showError(const String &message)
  {
    show("System Error", message, "Check hardware", "or SD card");
  }

  bool isReady()
  {
    return ready;
  }
}
