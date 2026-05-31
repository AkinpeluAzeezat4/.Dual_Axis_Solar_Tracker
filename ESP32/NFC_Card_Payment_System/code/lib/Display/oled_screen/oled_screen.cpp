#include "oled_screen.h"
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "battery_level/battery_level.h"
#include "WiFi/WiFi.h"

namespace
{
  U8G2_SSD1309_128X64_NONAME0_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

  void drawBatteryBar()
  {
    uint8_t percent = battery_level::getPercentage();

    uint8_t x = 116;
    uint8_t y = 1;
    uint8_t w = 10;
    uint8_t h = 5;

    u8g2.drawFrame(x, y, w, h);
    u8g2.drawBox(x + w, y + 1, 1, 3);

    uint8_t fillWidth = (percent * (w - 2)) / 100;
    u8g2.drawBox(x + 1, y + 1, fillWidth, h - 2);
  }

  void drawNetworkIcon()
  {
    uint8_t x = 100;
    uint8_t y = 1;
    bool connected = wifi_service::isConnected();

    u8g2.drawBox(x + 0, y + 4, 2, 1);
    u8g2.drawBox(x + 3, y + 3, 2, 2);

    if (connected)
    {
      u8g2.drawBox(x + 6, y + 2, 2, 3);
      u8g2.drawBox(x + 9, y + 1, 2, 4);
    }
    else
    {
      u8g2.drawBox(x + 6, y + 2, 2, 3);
      u8g2.drawLine(x - 5, y + 1, x - 2, y + 4);
      u8g2.drawLine(x - 2, y + 1, x - 5, y + 4);
    }
  }

  void drawTopIcons()
  {
    drawNetworkIcon();
    drawBatteryBar();
  }

  void drawTapToPayZoom()
  {
    static unsigned long lastAnimUpdate = 0;
    static uint8_t animStep = 0;

    if (millis() - lastAnimUpdate >= 120)
    {
      lastAnimUpdate = millis();
      animStep++;
      if (animStep > 5)
      {
        animStep = 0;
      }
    }

    const char *text = "TAP TO PAY";
    int textX = 0;
    int textY = 0;

    switch (animStep)
    {
    case 0:
      u8g2.setFont(u8g2_font_6x12_tf);
      textX = (128 - u8g2.getStrWidth(text)) / 2;
      textY = 26;
      break;

    case 1:
      u8g2.setFont(u8g2_font_7x14B_tf);
      textX = (128 - u8g2.getStrWidth(text)) / 2;
      textY = 28;
      break;

    case 2:
      u8g2.setFont(u8g2_font_courB12_tf);
      textX = (128 - u8g2.getStrWidth(text)) / 2;
      textY = 30;
      break;

    default:
      u8g2.setFont(u8g2_font_courB14_tf);
      textX = (128 - u8g2.getStrWidth(text)) / 2;
      textY = 34;
      break;
    }

    u8g2.drawStr(textX, textY, text);
  }

  void drawCenteredRoundedMessage(const char *line1, const char *line2, bool inverted)
  {
    uint8_t boxX = 14;
    uint8_t boxY = 12;
    uint8_t boxW = 100;
    uint8_t boxH = 42;
    uint8_t radius = 4;

    if (inverted)
    {
      u8g2.drawRBox(boxX, boxY, boxW, boxH, radius);
      u8g2.setDrawColor(0);
    }
    else
    {
      u8g2.drawRFrame(boxX, boxY, boxW, boxH, radius);
      u8g2.setDrawColor(1);
    }

    u8g2.setFont(u8g2_font_7x14B_tf);

    int textX1 = boxX + (boxW - u8g2.getStrWidth(line1)) / 2;
    int textX2 = boxX + (boxW - u8g2.getStrWidth(line2)) / 2;

    u8g2.drawStr(textX1, 31, line1);
    u8g2.drawStr(textX2, 45, line2);

    u8g2.setDrawColor(1);
  }
}

namespace oled_screen
{
  void begin(uint8_t sdaPin, uint8_t sclPin)
  {
    Wire.begin(sdaPin, sclPin, 100000);
    u8g2.setI2CAddress(0x78);
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.sendBuffer();
  }

  void update(const char *command, const char *amount)
  {
    if (command == nullptr)
    {
      return;
    }

    u8g2.clearBuffer();
    drawTopIcons();

    if (strcmp(command, "welcome") == 0)
    {
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(17, 20, "NFC Card Payment");
      u8g2.drawStr(47, 34, "System");
      u8g2.drawLine(12, 42, 116, 42);

      static uint8_t dots = 0;
      static unsigned long lastUpdate = 0;

      if (millis() - lastUpdate >= 400)
      {
        lastUpdate = millis();
        dots = (dots + 1) % 4;
      }

      if (dots >= 1)
        u8g2.drawStr(56, 57, ".");
      if (dots >= 2)
        u8g2.drawStr(62, 57, ".");
      if (dots >= 3)
        u8g2.drawStr(68, 57, ".");
    }
    else if (strcmp(command, "main") == 0)
    {
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(4, 11, "HOME");
      u8g2.drawLine(0, 14, 127, 14);

      u8g2.setFont(u8g2_font_7x14B_tf);
      u8g2.drawStr(18, 28, "SELECT AMOUNT");

      const char *text = (amount != nullptr) ? amount : "N0";
      uint8_t boxX = 29;
      uint8_t boxY = 32;
      uint8_t boxW = 70;
      uint8_t boxH = 20;
      uint8_t radius = 3;

      u8g2.drawRFrame(boxX, boxY, boxW, boxH, radius);
      int textX = boxX + (boxW - u8g2.getStrWidth(text)) / 2;
      u8g2.drawStr(textX, 47, text);

      u8g2.setFont(u8g2_font_5x8_tf);
      u8g2.drawStr(10, 63, "UP/DOWN: Amt   OK: Pay");
    }
    else if (strcmp(command, "pay") == 0)
    {
      drawTapToPayZoom();
      u8g2.setFont(u8g2_font_6x12_tf);

      static unsigned long lastSwitch = 0;
      static bool showAmount = true;

      if (millis() - lastSwitch >= 2000)
      {
        lastSwitch = millis();
        showAmount = !showAmount;
      }

      uint8_t boxX = 33;
      uint8_t boxY = 48;
      uint8_t boxW = 60;
      uint8_t boxH = 14;
      uint8_t radius = 3;

      u8g2.drawRBox(boxX, boxY, boxW, boxH, radius);
      u8g2.setDrawColor(0);

      if (showAmount && amount != nullptr)
      {
        int textX = boxX + (boxW - u8g2.getStrWidth(amount)) / 2;
        u8g2.drawStr(textX, 59, amount);
      }
      else
      {
        const char *label = "Cancel";
        int textX = boxX + (boxW - u8g2.getStrWidth(label)) / 2;
        u8g2.drawStr(textX, 59, label);
      }

      u8g2.setDrawColor(1);
    }
    else if (strcmp(command, "payment_successful") == 0)
    {
      drawCenteredRoundedMessage("PAYMENT", "SUCCESSFUL", true);
    }
    else if (strcmp(command, "payment_failure") == 0)
    {
      drawCenteredRoundedMessage("PAYMENT", "FAILED", false);

      if (amount != nullptr)
      {
        u8g2.setFont(u8g2_font_5x8_tf);
        int x = (128 - u8g2.getStrWidth(amount)) / 2;
        u8g2.drawStr(x, 61, amount);
      }
    }
    else if (strcmp(command, "register_needed") == 0)
    {
      drawCenteredRoundedMessage("CARD NOT", "REGISTERED", false);
    }
    else if (strcmp(command, "card_registered") == 0)
    {
      drawCenteredRoundedMessage("CARD", "REGISTERED", true);
    }
    else if (strcmp(command, "error") == 0)
    {
      drawCenteredRoundedMessage("SYSTEM", "FAULT", false);

      if (amount != nullptr)
      {
        u8g2.setFont(u8g2_font_5x8_tf);
        int x = (128 - u8g2.getStrWidth(amount)) / 2;
        u8g2.drawStr(x, 61, amount);
      }
    }

    u8g2.sendBuffer();
  }
}
