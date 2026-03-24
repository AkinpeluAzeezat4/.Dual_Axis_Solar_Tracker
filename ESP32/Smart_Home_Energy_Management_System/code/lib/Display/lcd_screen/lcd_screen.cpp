#include "lcd_screen.h"
#include <U8g2lib.h>
#include "load_relay/load_relay.h"
#include "rotary_encoder/rotary_encoder.h"
#include "shared_var/shared_var.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R2, 5, U8X8_PIN_NONE);

namespace lcd_screen
{
  enum ScreenState
  {
    MENU,
    RELAY_SETUP,
    TIMER_SETUP,
    WIFI_SETUP,
    SYSTEM_SETUP,
    EXIT
  };
  unsigned long lastMenuMove = 0;
  const unsigned long menuMoveInterval = 150;
  ScreenState currentScreen = MENU;

  static unsigned long lastRelayMove = 0;
  const unsigned long relayMoveInterval = 100;

  int relayPower[6] = {0, 0, 0, 0, 0, 0};
  int relayIndex = 0;

  int systemValues[2] = {0, 0};
  int systemIndex = 0;

  const char *systemItems[2] = {
      "Inverter Power",
      "System Power"};

  bool setupFinished = false;

  int selectedItem = 0;
  const int totalItems = 5;

  const char *menuItems[totalItems] =
      {
          "Relay Mode",
          "Timer Setup",
          "WiFi Config",
          "System Setup",
          "Exit"};

  int lastPosition = 0;

  void begin()
  {
    u8g2.begin();
  }

  void update(const char *command)
  {
    if (currentScreen == RELAY_SETUP)
    {
      relayPowerSetup();
      return;
    }
    else if (currentScreen == TIMER_SETUP)
    {
      return;
    }
    else if (currentScreen == WIFI_SETUP)
    {
      return;
    }
    else if (currentScreen == SYSTEM_SETUP)
    {
      systemSettings();
      return;
    }
    else if (currentScreen == EXIT)
    {
      // lcd_screen::update("loadStatus");
      shared_var::settingsMode = false;
      selectedItem = 0;
      setupFinished = true;
      currentScreen = MENU;
      // currentScreen = RELAY_SETUP;
    }

    u8g2.firstPage();
    do
    {
      if (strcmp(command, "loadStatus") == 0)
      {

        drawLoadStatusScreen();
      }
      else if (strcmp(command, "loadConsumption") == 0)
      {
        drawConsumptionScreen();
      }
      else if (strcmp(command, "settings") == 0)
      {
        updateMenu();
        drawSettingsScreen();
      }
    } while (u8g2.nextPage());

    if (rotary_encoder::wasPressed() && shared_var::settingsMode)
    {
      if (selectedItem == 0)
      {
        currentScreen = RELAY_SETUP;
      }
      else if (selectedItem == 1)
      {
        currentScreen = TIMER_SETUP;
      }
      else if (selectedItem == 2)
      {
        currentScreen = WIFI_SETUP;
      }
      else if (selectedItem == 3)
      {
        currentScreen = SYSTEM_SETUP;
      }
      else if (selectedItem == 4)
      {
        currentScreen = EXIT;
      }
    }
  }

  void drawLoadStatusScreen()
  {
    u8g2.setDrawColor(1);
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.setCursor(11, 8);
    u8g2.print("SMART HOME AUTOMATION");
    u8g2.setCursor(52, 16);
    u8g2.print("SYSTEM");
    u8g2.drawFrame(0, 18, 128, 1);

    drawSwitchStatus(8, 30, 1, load_relay::getRelay1());
    drawSwitchStatus(49, 30, 2, load_relay::getRelay2());
    drawSwitchStatus(91, 30, 3, load_relay::getRelay3());
    drawSwitchStatus(8, 51, 4, load_relay::getRelay4());
    drawSwitchStatus(49, 51, 5, load_relay::getRelay5());
    drawSwitchStatus(91, 51, 6, load_relay::getRelay6());
  }

  // void drawConsumptionScreen()
  // {
  //   u8g2.setDrawColor(1);
  //   u8g2.drawFrame(0, 0, 128, 64);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(11, 8);
  //   u8g2.print("SMART HOME AUTOMATION");
  //   u8g2.setCursor(52, 16);
  //   u8g2.print("SYSTEM");
  //   u8g2.drawFrame(0, 18, 128, 1);

  //   u8g2.drawBox(10, 20, 50, 9);

  //   u8g2.drawFrame(10, 20, 50, 20);

  //   u8g2.setDrawColor(0);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(23, 28);
  //   u8g2.print("VOLTAGE");

  //   u8g2.setDrawColor(1);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(23, 37);
  //   u8g2.print("220.45");
  //   u8g2.print("V");

  //   u8g2.drawBox(69, 20, 50, 9);

  //   u8g2.drawFrame(69, 20, 50, 20);

  //   u8g2.setDrawColor(0);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(72, 28);
  //   u8g2.print("CURRENT");

  //   u8g2.setDrawColor(1);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(72, 37);
  //   u8g2.print("30.45");
  //   u8g2.print("A");

  //   u8g2.drawBox(10, 42, 50, 9);

  //   u8g2.drawFrame(10, 42, 50, 20);

  //   u8g2.setDrawColor(0);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(29, 50);
  //   u8g2.print("POWER");

  //   u8g2.setDrawColor(1);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(23, 59);
  //   u8g2.print("540.45");
  //   u8g2.print("W");

  //   u8g2.drawBox(69, 42, 50, 9);

  //   u8g2.drawFrame(69, 42, 50, 20);

  //   u8g2.setDrawColor(0);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(74, 50);
  //   u8g2.print("ENERGY");

  //   u8g2.setDrawColor(1);
  //   u8g2.setFont(u8g2_font_5x8_tr);
  //   u8g2.setCursor(72, 59);
  //   u8g2.print("456.78");
  //   u8g2.print("Wh");
  // }

  void drawCenteredText(int boxX, int boxY, int boxW, int boxH, const char *text)
  {
    int textWidth = u8g2.getStrWidth(text);
    int x = boxX + (boxW - textWidth) / 2;
    int y = boxY + (boxH / 2) + 3;

    u8g2.setCursor(x, y);
    u8g2.print(text);
  }

  void drawConsumptionScreen()
  {
    // u8g2.setDrawColor(1);
    // u8g2.drawFrame(0, 0, 128, 64);

    // u8g2.setFont(u8g2_font_5x8_tr);
    // drawCenteredText(0, 0, 128, 16, "SMART HOME AUTOMATION");
    // drawCenteredText(0, 8, 128, 16, "SYSTEM");

    // u8g2.drawFrame(0, 18, 128, 1);

    u8g2.setDrawColor(1);
    u8g2.drawFrame(0, 0, 128, 64);
    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.setCursor(11, 8);
    u8g2.print("SMART HOME AUTOMATION");
    u8g2.setCursor(52, 16);
    u8g2.print("SYSTEM");
    u8g2.drawFrame(0, 18, 128, 1);

    // BOX 1 - VOLTAGE
    u8g2.drawBox(10, 20, 50, 9);
    u8g2.drawFrame(10, 20, 50, 20);

    u8g2.setDrawColor(0);
    drawCenteredText(10, 20, 50, 9, "VOLTAGE");

    u8g2.setDrawColor(1);
    drawCenteredText(10, 30, 50, 10, "0.45");
    u8g2.print("V");
    // BOX 2 - CURRENT
    u8g2.drawBox(69, 20, 50, 9);
    u8g2.drawFrame(69, 20, 50, 20);

    u8g2.setDrawColor(0);
    drawCenteredText(69, 20, 50, 9, "CURRENT");

    u8g2.setDrawColor(1);
    drawCenteredText(69, 30, 50, 10, "3.45");
    u8g2.print("A");
    // BOX 3 - POWER
    u8g2.drawBox(10, 42, 50, 9);
    u8g2.drawFrame(10, 42, 50, 20);

    u8g2.setDrawColor(0);
    drawCenteredText(10, 42, 50, 9, "POWER");

    u8g2.setDrawColor(1);
    drawCenteredText(10, 52, 50, 10, "4.45");
    u8g2.print("W");
    // BOX 4 - ENERGY
    u8g2.drawBox(69, 42, 50, 9);
    u8g2.drawFrame(69, 42, 50, 20);

    u8g2.setDrawColor(0);
    drawCenteredText(69, 42, 50, 9, "ENERGY");

    u8g2.setDrawColor(1);
    drawCenteredText(69, 52, 50, 10, "6.78");
    u8g2.print("Wh");
  }

  void drawSettingsScreen()
  {
    // Title
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(37, 9);
    u8g2.print("SETTINGS");
    u8g2.drawFrame(33, 10, 63, 1);

    // Menu
    u8g2.setFont(u8g2_font_5x8_tr);

    for (int i = 0; i < totalItems; i++)
    {
      int y = 20 + (i * 10);

      if (i == selectedItem)
      {
        u8g2.drawBox(5, y - 8, 118, 11);
        u8g2.setDrawColor(0);
        u8g2.setCursor(10, y);
        u8g2.print(menuItems[i]);
        u8g2.setDrawColor(1);
      }
      else
      {
        u8g2.setCursor(10, y);
        u8g2.print(menuItems[i]);
      }
    }
  }

  void updateMenu()
  {
    int dir = rotary_encoder::getDirection();
    unsigned long now = millis();

    if (dir != 0 && (now - lastMenuMove >= menuMoveInterval))
    {
      selectedItem += dir;
      selectedItem = (selectedItem + totalItems) % totalItems; // wrap-around safely
      lastMenuMove = now;                                      // remember when we last moved
    }
  }
  void drawRoundedFrame(int x, int y, int w, int h, int r)
  {
    u8g2.drawHLine(x + r, y, w - 2 * r);
    u8g2.drawHLine(x + r, y + h - 1, w - 2 * r);
    u8g2.drawVLine(x, y + r, h - 2 * r);
    u8g2.drawVLine(x + w - 1, y + r, h - 2 * r);

    u8g2.drawCircle(x + r, y + r, r, U8G2_DRAW_UPPER_LEFT);
    u8g2.drawCircle(x + w - r - 1, y + r, r, U8G2_DRAW_UPPER_RIGHT);
    u8g2.drawCircle(x + r, y + h - r - 1, r, U8G2_DRAW_LOWER_LEFT);
    u8g2.drawCircle(x + w - r - 1, y + h - r - 1, r, U8G2_DRAW_LOWER_RIGHT);
  }
  void drawSwitchStatus(int x, int y, int relayNum, bool state)
  {
    drawRoundedFrame(x, y - 6, 30, 13, 3);

    char buf[4];
    itoa(relayNum, buf, 10);
    if (state)
    {
      u8g2.drawDisc(x + 23, y, 4);

      u8g2.setFont(u8g2_font_4x6_tr);

      uint8_t w = u8g2.getStrWidth(buf);
      u8g2.setCursor(x + 23 - (w / 2), y + 3);
      u8g2.setDrawColor(0);
      u8g2.print(buf);
      u8g2.setDrawColor(1);

      u8g2.setFont(u8g2_font_5x8_tr);
      u8g2.setCursor(x + 2, y + 4);
      u8g2.print("INV");
    }
    else
    {
      u8g2.drawDisc(x + 6, y, 4);
      u8g2.setFont(u8g2_font_4x6_tr);

      uint8_t w = u8g2.getStrWidth(buf);
      u8g2.setCursor(x + 6 - (w / 2), y + 3);
      u8g2.setDrawColor(0);
      u8g2.print(buf);
      u8g2.setDrawColor(1);

      u8g2.setFont(u8g2_font_5x8_tr);
      u8g2.setCursor(x + 12, y + 4);
      u8g2.print("GRD");
    }
  }
  int getSelectedItem()
  {
    return selectedItem;
  }
  void relayPowerSetup()
  {
    unsigned long now = millis();
    int dir = rotary_encoder::getDirection();

    if (dir != 0 && (now - lastRelayMove >= relayMoveInterval))
    {
      if (dir > 0)
        relayPower[relayIndex] += 50;
      else
        relayPower[relayIndex] -= 50;

      // Clamp values
      if (relayPower[relayIndex] < 0)
        relayPower[relayIndex] = 0;
      if (relayPower[relayIndex] > 4000)
        relayPower[relayIndex] = 4000;

      lastRelayMove = now;
    }

    static bool encoderPrevPressed = false;

    bool encoderPressed = rotary_encoder::wasPressed();
    if (encoderPressed && !encoderPrevPressed)
    {
      relayIndex++;
      if (relayIndex >= 6)
      {
        relayIndex = 0;
        setupFinished = true;
        currentScreen = MENU;
        return;
      }
    }
    encoderPrevPressed = encoderPressed;
    drawRelaySetupScreen();
  }

  void systemSettings()
  {
    unsigned long now = millis();
    int dir = rotary_encoder::getDirection();

    if (dir != 0 && (now - lastRelayMove >= relayMoveInterval))
    {
      if (dir > 0)
        systemValues[systemIndex] += 100;
      else
        systemValues[systemIndex] -= 100;

      if (systemValues[systemIndex] < 0)
        systemValues[systemIndex] = 0;

      if (systemValues[systemIndex] > 10000)
        systemValues[systemIndex] = 10000;

      lastRelayMove = now;
    }

    static bool encoderPrevPressed = false;
    bool encoderPressed = rotary_encoder::wasPressed();

    if (encoderPressed && !encoderPrevPressed)
    {
      systemIndex++;

      if (systemIndex >= 2)
      {
        systemIndex = 0;
        setupFinished = true;
        currentScreen = MENU;
        return;
      }
    }

    encoderPrevPressed = encoderPressed;

    drawSystemSettingsScreen();
  }

  void drawRelaySetupScreen()
  {
    u8g2.firstPage();
    do
    {
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.setCursor(13, 9);
      u8g2.print("RELAY POWER SETUP");
      u8g2.drawFrame(13, 10, 100, 1);
      u8g2.setFont(u8g2_font_5x8_tr);

      for (int i = 0; i < 6; i++)
      {
        int y = 20 + (i * 8);

        if (i == relayIndex)
        {
          u8g2.drawBox(0, y - 7, 128, 8);
          u8g2.setDrawColor(0);
        }

        u8g2.setCursor(5, y);
        u8g2.print("Relay ");
        u8g2.print(i + 1);

        u8g2.setCursor(70, y);
        u8g2.print(relayPower[i]);
        u8g2.print("W");

        u8g2.setDrawColor(1);
      }
    }

    while (u8g2.nextPage());
  }

  void drawSystemSettingsScreen()
  {
    u8g2.firstPage();
    do
    {
      u8g2.setFont(u8g2_font_6x10_tr);
      u8g2.setCursor(20, 9);
      u8g2.print("SYSTEM SETUP");
      u8g2.drawFrame(20, 10, 90, 1);

      u8g2.setFont(u8g2_font_5x8_tr);

      for (int i = 0; i < 2; i++)
      {
        int y = 25 + (i * 12);

        if (i == systemIndex)
        {
          u8g2.drawBox(0, y - 8, 128, 12);
          u8g2.setDrawColor(0);
        }

        u8g2.setCursor(5, y);
        u8g2.print(systemItems[i]);

        u8g2.setCursor(90, y);
        u8g2.print(systemValues[i]);
        u8g2.print("W");

        u8g2.setDrawColor(1);
      }

    } while (u8g2.nextPage());
  }

  bool isRelaySetupFinished()
  {
    return setupFinished;
  }
  void resetRelaySetup()
  {
    setupFinished = false;
  }
  int getRelayPower(int relay)
  {
    if (relay < 0 || relay > 5)
      return 0;
    return relayPower[relay];
  }
  int getInverterPower()
  {
    return systemValues[0];
  }

  int getSystemPower()
  {
    return systemValues[1];
  }
}

// u8g2.setFont(u8g2_font_4x6_tr); Very Small (Tiny UI labels)
// u8g2.setFont(u8g2_font_5x8_tr); small
// u8g2.setFont(u8g2_font_6x10_tr); medium
// u8g2.setFont(u8g2_font_7x13_tr); slightly bigger
// u8g2.setFont(u8g2_font_logisoso16_tr); large
// u8g2.setFont(u8g2_font_logisoso20_tr); bigger
// u8g2.setFont(u8g2_font_logisoso24_tr); very large
// u8g2.setFont(u8g2_font_ncenB14_tr); bold block
