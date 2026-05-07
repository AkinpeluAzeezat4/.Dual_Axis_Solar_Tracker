#include <Arduino.h>
#include <Preferences.h>
#include "settings_manager.h"

namespace settings_manager
{
  static Preferences prefs;
  static Settings data;

  static void loadDefaults()
  {
    data.tankEmptyCm = 80.0f;
    data.tankFullCm = 12.0f;

    data.soilDryRaw = 3200;
    data.soilWetRaw = 1400;

    data.batteryEmptyV = 3.20f;
    data.batteryFullV = 4.20f;
    data.batteryScaleFactor = 2.0f;

    data.tankLowPercent = 25;
    data.tankHighPercent = 90;
    data.soilDryPercent = 35;
    data.batteryLowPercent = 15;

    data.irrigationRunSeconds = 15;
    data.irrigationPauseSeconds = 60;
    data.tankPumpMaxMinutes = 8;

    data.valid = true;
  }

  void begin()
  {
    prefs.begin("irrigation", false);

    bool ok = prefs.getBool("valid", false);

    if (!ok)
    {
      loadDefaults();
      save();
      return;
    }

    data.tankEmptyCm = prefs.getFloat("tankEmpty", 80.0f);
    data.tankFullCm = prefs.getFloat("tankFull", 12.0f);

    data.soilDryRaw = prefs.getUShort("soilDry", 3200);
    data.soilWetRaw = prefs.getUShort("soilWet", 1400);

    data.batteryEmptyV = prefs.getFloat("batEmpty", 3.20f);
    data.batteryFullV = prefs.getFloat("batFull", 4.20f);
    data.batteryScaleFactor = prefs.getFloat("batScale", 2.0f);

    data.tankLowPercent = prefs.getUShort("tankLow", 25);
    data.tankHighPercent = prefs.getUShort("tankHigh", 90);
    data.soilDryPercent = prefs.getUShort("soilPct", 35);
    data.batteryLowPercent = prefs.getUShort("batLowPct", 15);

    data.irrigationRunSeconds = prefs.getUShort("irrRun", 15);
    data.irrigationPauseSeconds = prefs.getUShort("irrPause", 60);
    data.tankPumpMaxMinutes = prefs.getUShort("pumpMax", 8);

    data.valid = true;
  }

  void save()
  {
    prefs.putFloat("tankEmpty", data.tankEmptyCm);
    prefs.putFloat("tankFull", data.tankFullCm);

    prefs.putUShort("soilDry", data.soilDryRaw);
    prefs.putUShort("soilWet", data.soilWetRaw);

    prefs.putFloat("batEmpty", data.batteryEmptyV);
    prefs.putFloat("batFull", data.batteryFullV);
    prefs.putFloat("batScale", data.batteryScaleFactor);

    prefs.putUShort("tankLow", data.tankLowPercent);
    prefs.putUShort("tankHigh", data.tankHighPercent);
    prefs.putUShort("soilPct", data.soilDryPercent);
    prefs.putUShort("batLowPct", data.batteryLowPercent);

    prefs.putUShort("irrRun", data.irrigationRunSeconds);
    prefs.putUShort("irrPause", data.irrigationPauseSeconds);
    prefs.putUShort("pumpMax", data.tankPumpMaxMinutes);

    prefs.putBool("valid", true);
  }

  void resetDefaults()
  {
    loadDefaults();
    save();
  }

  Settings &get()
  {
    return data;
  }
}
