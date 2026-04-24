#include "sun_sensor.h"
#include "pins.h"
#include <Preferences.h>

namespace sun_sensor
{
  SensorData data;
  Preferences prefs;

  int nightThreshold = 950;
  int deadbandOffset = 0;

  unsigned long lastUpdate = 0;

  const unsigned long sampleInterval = 80;
  const float filterAlpha = 0.18f;

  int filteredRead(uint8_t pin, int previous)
  {
    int raw = Pins::readAnalog(pin);

    if (previous <= 0)
      return raw;

    return (int)((previous * (1.0f - filterAlpha)) + (raw * filterAlpha));
  }

  int clampValue(int value, int low, int high)
  {
    if (value < low)
      return low;

    if (value > high)
      return high;

    return value;
  }

  void begin()
  {
    prefs.begin("solar", false);

    nightThreshold = prefs.getInt("night", 950);
    deadbandOffset = prefs.getInt("dbOffset", 0);

    data.topLeft = Pins::readAnalog(Pins::LDR_TOP_LEFT);
    data.topRight = Pins::readAnalog(Pins::LDR_TOP_RIGHT);
    data.bottomLeft = Pins::readAnalog(Pins::LDR_BOTTOM_LEFT);
    data.bottomRight = Pins::readAnalog(Pins::LDR_BOTTOM_RIGHT);
    data.night = Pins::readAnalog(Pins::LDR_NIGHT);
    data.pot = Pins::readAnalog(Pins::POT_CALIBRATION);

    data.leftAverage = 0;
    data.rightAverage = 0;
    data.topAverage = 0;
    data.bottomAverage = 0;
    data.horizontalError = 0;
    data.verticalError = 0;
    data.strongestIndex = 0;
    data.isDark = false;

    lastUpdate = millis();
  }

  void update()
  {
    unsigned long now = millis();

    if (now - lastUpdate < sampleInterval)
      return;

    lastUpdate = now;

    data.topLeft = filteredRead(Pins::LDR_TOP_LEFT, data.topLeft);
    data.topRight = filteredRead(Pins::LDR_TOP_RIGHT, data.topRight);
    data.bottomLeft = filteredRead(Pins::LDR_BOTTOM_LEFT, data.bottomLeft);
    data.bottomRight = filteredRead(Pins::LDR_BOTTOM_RIGHT, data.bottomRight);
    data.night = filteredRead(Pins::LDR_NIGHT, data.night);
    data.pot = filteredRead(Pins::POT_CALIBRATION, data.pot);

    data.leftAverage = (data.topLeft + data.bottomLeft) / 2;
    data.rightAverage = (data.topRight + data.bottomRight) / 2;
    data.topAverage = (data.topLeft + data.topRight) / 2;
    data.bottomAverage = (data.bottomLeft + data.bottomRight) / 2;

    data.horizontalError = data.leftAverage - data.rightAverage;
    data.verticalError = data.topAverage - data.bottomAverage;

    int values[4] = {
        data.topLeft,
        data.topRight,
        data.bottomLeft,
        data.bottomRight};

    int best = 0;

    for (int i = 1; i < 4; i++)
    {
      if (values[i] > values[best])
        best = i;
    }

    data.strongestIndex = best;
    data.isDark = data.night < nightThreshold;
  }

  SensorData getData()
  {
    return data;
  }

  int getDeadband()
  {
    int base = map(data.pot, 0, 4095, 80, 650);
    return clampValue(base + deadbandOffset, 30, 1200);
  }

  int getNightThreshold()
  {
    return nightThreshold;
  }

  void setNightThreshold(int value)
  {
    nightThreshold = clampValue(value, 50, 3500);
    prefs.putInt("night", nightThreshold);
  }

  void setDeadbandOffset(int value)
  {
    deadbandOffset = clampValue(value, -300, 500);
    prefs.putInt("dbOffset", deadbandOffset);
  }

  int getDeadbandOffset()
  {
    return deadbandOffset;
  }
}
