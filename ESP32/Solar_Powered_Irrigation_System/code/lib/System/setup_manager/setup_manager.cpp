#include <Arduino.h>
#include "setup_manager.h"
#include "rotary_encoder/rotary_encoder.h"
#include "settings_manager/settings_manager.h"
#include "ultrasonic/ultrasonic.h"
#include "battery_level/battery_level.h"
#include "soil_sensor/soil_sensor.h"
#include "pump/pump.h"
#include "buzzer/buzzer.h"
#include "lcd_screen/lcd_screen.h"

namespace setup_manager
{
  enum MenuItem
  {
    SET_TANK_FULL,
    SET_TANK_EMPTY,
    SET_LOW_LEVEL,
    SET_FULL_STOP,
    SET_BAT_EMPTY,
    SET_BAT_FULL,
    SET_BAT_SCALE,
    SET_SOIL_DRY,
    SET_IRR_RUN,
    SET_IRR_PAUSE,
    SET_TANK_MAX,
    SAVE_EXIT,
    RESET_DEFAULTS,
    MENU_COUNT
  };

  static bool active = false;
  static bool editing = false;
  static int selected = 0;
  static unsigned long lastDraw = 0;
  static String notice = "";
  static unsigned long noticeUntil = 0;

  static int clampInt(int value, int low, int high)
  {
    if (value < low)
    {
      return low;
    }

    if (value > high)
    {
      return high;
    }

    return value;
  }

  static float clampFloat(float value, float low, float high)
  {
    if (value < low)
    {
      return low;
    }

    if (value > high)
    {
      return high;
    }

    return value;
  }

  static const char *itemName(MenuItem item)
  {
    switch (item)
    {
    case SET_TANK_FULL:
      return "Cal Tank Full";
    case SET_TANK_EMPTY:
      return "Cal Tank Empty";
    case SET_LOW_LEVEL:
      return "Low Tank Level";
    case SET_FULL_STOP:
      return "Full Stop Level";
    case SET_BAT_EMPTY:
      return "Battery Empty V";
    case SET_BAT_FULL:
      return "Battery Full V";
    case SET_BAT_SCALE:
      return "Battery Scale";
    case SET_SOIL_DRY:
      return "Soil Dry Level";
    case SET_IRR_RUN:
      return "Irr Run Time";
    case SET_IRR_PAUSE:
      return "Irr Pause Time";
    case SET_TANK_MAX:
      return "Tank Max Time";
    case SAVE_EXIT:
      return "Save And Exit";
    case RESET_DEFAULTS:
      return "Reset Defaults";
    default:
      return "Setup";
    }
  }

  static String itemValue(MenuItem item)
  {
    auto &s = settings_manager::get();
    char buffer[21];

    switch (item)
    {
    case SET_TANK_FULL:
      snprintf(buffer, sizeof(buffer), "Now %.1f Saved %.1f", ultrasonic::getDistanceCm(), s.tankFullCm);
      return String(buffer);
    case SET_TANK_EMPTY:
      snprintf(buffer, sizeof(buffer), "Now %.1f Saved %.1f", ultrasonic::getDistanceCm(), s.tankEmptyCm);
      return String(buffer);
    case SET_LOW_LEVEL:
      snprintf(buffer, sizeof(buffer), "%u%%", s.tankLowPercent);
      return String(buffer);
    case SET_FULL_STOP:
      snprintf(buffer, sizeof(buffer), "%u%%", s.tankHighPercent);
      return String(buffer);
    case SET_BAT_EMPTY:
      snprintf(buffer, sizeof(buffer), "%.2fV Now %.2f", s.batteryEmptyV, battery_level::getVoltage());
      return String(buffer);
    case SET_BAT_FULL:
      snprintf(buffer, sizeof(buffer), "%.2fV Now %.2f", s.batteryFullV, battery_level::getVoltage());
      return String(buffer);
    case SET_BAT_SCALE:
      snprintf(buffer, sizeof(buffer), "x%.2f", s.batteryScaleFactor);
      return String(buffer);
    case SET_SOIL_DRY:
      snprintf(buffer, sizeof(buffer), "%u%% Raw %d", s.soilDryPercent, soil_sensor::getRaw());
      return String(buffer);
    case SET_IRR_RUN:
      snprintf(buffer, sizeof(buffer), "%us", s.irrigationRunSeconds);
      return String(buffer);
    case SET_IRR_PAUSE:
      snprintf(buffer, sizeof(buffer), "%us", s.irrigationPauseSeconds);
      return String(buffer);
    case SET_TANK_MAX:
      snprintf(buffer, sizeof(buffer), "%umin", s.tankPumpMaxMinutes);
      return String(buffer);
    case SAVE_EXIT:
      return "Click to save";
    case RESET_DEFAULTS:
      return "Click to reset";
    default:
      return "";
    }
  }

  static bool isEditable(MenuItem item)
  {
    return item == SET_LOW_LEVEL || item == SET_FULL_STOP || item == SET_BAT_EMPTY || item == SET_BAT_FULL || item == SET_BAT_SCALE || item == SET_SOIL_DRY || item == SET_IRR_RUN || item == SET_IRR_PAUSE || item == SET_TANK_MAX;
  }

  static void showNotice(const String &text)
  {
    notice = text;
    noticeUntil = millis() + 1500UL;
  }

  static void draw(bool force)
  {
    if (!force && millis() - lastDraw < 300UL)
    {
      return;
    }

    lastDraw = millis();

    MenuItem item = (MenuItem)selected;
    String line0 = editing ? "SETUP EDIT MODE" : "SETUP MODE";
    String line1 = String(selected + 1) + "/" + String(MENU_COUNT) + " " + itemName(item);
    String line2 = itemValue(item);
    String line3;

    if (notice.length() > 0 && millis() < noticeUntil)
    {
      line3 = notice;
    }
    else
    {
      notice = "";

      if (editing)
      {
        line3 = "Turn=Edit Click=OK";
      }
      else if (item == SET_TANK_FULL || item == SET_TANK_EMPTY)
      {
        line3 = "Click=Capture";
      }
      else if (item == SAVE_EXIT || item == RESET_DEFAULTS)
      {
        line3 = "Click=Run";
      }
      else
      {
        line3 = "Click=Edit Hold=Exit";
      }
    }

    lcd_screen::showMessage(line0.c_str(), line1.c_str(), line2.c_str(), line3.c_str());
  }

  static void enter()
  {
    active = true;
    editing = false;
    selected = 0;
    notice = "";
    pump::allOff();
    lcd_screen::setAutoRefresh(false);
    rotary_encoder::getDelta();
    buzzer::beep(150);
    draw(true);
  }

  static void exitAndSave()
  {
    settings_manager::save();
    active = false;
    editing = false;
    lcd_screen::setAutoRefresh(true);
    buzzer::beep(150);
  }

  static void adjustValue(MenuItem item, int delta)
  {
    if (delta == 0)
    {
      return;
    }

    auto &s = settings_manager::get();

    switch (item)
    {
    case SET_LOW_LEVEL:
      s.tankLowPercent = clampInt((int)s.tankLowPercent + delta, 5, 80);
      if (s.tankHighPercent <= s.tankLowPercent)
      {
        s.tankHighPercent = s.tankLowPercent + 5;
      }
      break;
    case SET_FULL_STOP:
      s.tankHighPercent = clampInt((int)s.tankHighPercent + delta, s.tankLowPercent + 5, 100);
      break;
    case SET_BAT_EMPTY:
      s.batteryEmptyV = clampFloat(s.batteryEmptyV + (delta * 0.05f), 2.50f, 60.00f);
      if (s.batteryFullV <= s.batteryEmptyV)
      {
        s.batteryFullV = s.batteryEmptyV + 0.10f;
      }
      break;
    case SET_BAT_FULL:
      s.batteryFullV = clampFloat(s.batteryFullV + (delta * 0.05f), s.batteryEmptyV + 0.10f, 60.00f);
      break;
    case SET_BAT_SCALE:
      s.batteryScaleFactor = clampFloat(s.batteryScaleFactor + (delta * 0.05f), 1.00f, 20.00f);
      break;
    case SET_SOIL_DRY:
      s.soilDryPercent = clampInt((int)s.soilDryPercent + delta, 5, 95);
      break;
    case SET_IRR_RUN:
      s.irrigationRunSeconds = clampInt((int)s.irrigationRunSeconds + (delta * 5), 5, 600);
      break;
    case SET_IRR_PAUSE:
      s.irrigationPauseSeconds = clampInt((int)s.irrigationPauseSeconds + (delta * 10), 10, 3600);
      break;
    case SET_TANK_MAX:
      s.tankPumpMaxMinutes = clampInt((int)s.tankPumpMaxMinutes + delta, 1, 60);
      break;
    default:
      break;
    }
  }

  static void captureTankFull()
  {
    if (!ultrasonic::isReady())
    {
      showNotice("Sensor not ready");
      buzzer::beep(300);
      return;
    }

    auto &s = settings_manager::get();
    float cm = ultrasonic::getDistanceCm();

    if (cm < 2.0f || cm >= s.tankEmptyCm - 3.0f)
    {
      showNotice("Invalid full cm");
      buzzer::beep(300);
      return;
    }

    s.tankFullCm = cm;
    settings_manager::save();
    showNotice("Full level saved");
    buzzer::beep(120);
  }

  static void captureTankEmpty()
  {
    if (!ultrasonic::isReady())
    {
      showNotice("Sensor not ready");
      buzzer::beep(300);
      return;
    }

    auto &s = settings_manager::get();
    float cm = ultrasonic::getDistanceCm();

    if (cm <= s.tankFullCm + 3.0f)
    {
      showNotice("Invalid empty cm");
      buzzer::beep(300);
      return;
    }

    s.tankEmptyCm = cm;
    settings_manager::save();
    showNotice("Empty level saved");
    buzzer::beep(120);
  }

  static void handleClick()
  {
    MenuItem item = (MenuItem)selected;

    if (item == SET_TANK_FULL)
    {
      captureTankFull();
      return;
    }

    if (item == SET_TANK_EMPTY)
    {
      captureTankEmpty();
      return;
    }

    if (item == SAVE_EXIT)
    {
      exitAndSave();
      return;
    }

    if (item == RESET_DEFAULTS)
    {
      settings_manager::resetDefaults();
      editing = false;
      showNotice("Defaults loaded");
      buzzer::beep(150);
      return;
    }

    if (isEditable(item))
    {
      editing = !editing;

      if (!editing)
      {
        settings_manager::save();
        showNotice("Saved");
        buzzer::beep(100);
      }
      else
      {
        buzzer::beep(80);
      }
    }
  }

  void begin()
  {
  }

  void update()
  {
    if (!active)
    {
      if (rotary_encoder::wasLongPressed())
      {
        enter();
      }

      return;
    }

    pump::allOff();

    if (rotary_encoder::wasLongPressed())
    {
      exitAndSave();
      return;
    }

    int delta = rotary_encoder::getDelta();

    if (delta != 0)
    {
      if (editing)
      {
        adjustValue((MenuItem)selected, delta);
      }
      else
      {
        selected += delta;

        while (selected < 0)
        {
          selected += MENU_COUNT;
        }

        while (selected >= MENU_COUNT)
        {
          selected -= MENU_COUNT;
        }
      }

      draw(true);
    }

    if (rotary_encoder::wasClicked())
    {
      handleClick();
      draw(true);
    }

    draw(false);
  }

  bool isActive()
  {
    return active;
  }
}
