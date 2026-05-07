#include <Arduino.h>
#include <math.h>
#include "load_manager.h"
#include "load_relay/load_relay.h"
#include "config_manager/config_manager.h"
#include "pzem_sensor/pzem_sensor.h"
#include "nepa_sense/nepa_sense.h"
#include "inverter_sense/inverter_sense.h"

namespace load_manager
{
  bool onInverter[6] = {true, true, true, true, true, true};
  bool loadEnabled[6] = {true, true, true, true, true, true};

  float loadRatio = 0.0f;
  float fuzzyRisk = 0.0f;

  int currentLoad = 0;
  int effectiveLimit = 0;

  const char *decisionText = "STARTING";

  unsigned long lastUpdate = 0;
  const unsigned long updateInterval = 500;

  float clampFloat(float value, float low, float high)
  {
    if (value < low)
      return low;

    if (value > high)
      return high;

    return value;
  }

  float risingMembership(float x, float a, float b)
  {
    if (x <= a)
      return 0.0f;

    if (x >= b)
      return 1.0f;

    return (x - a) / (b - a);
  }

  float fallingMembership(float x, float a, float b)
  {
    if (x <= a)
      return 1.0f;

    if (x >= b)
      return 0.0f;

    return (b - x) / (b - a);
  }

  void setAllLoads(bool enabled, bool inverter)
  {
    for (int i = 0; i < 6; i++)
    {
      loadEnabled[i] = enabled;
      onInverter[i] = inverter;
    }
  }

  void chooseLoadsForInverter(int limit)
  {
    int power[6];

    for (int i = 0; i < 6; i++)
      power[i] = config_manager::getRelayPower(i);

    int bestMask = 0;
    long bestScore = -2147483647L;

    for (int mask = 0; mask < 64; mask++)
    {
      int sum = 0;
      long priorityScore = 0;

      for (int i = 0; i < 6; i++)
      {
        if (mask & (1 << i))
        {
          sum += power[i];
          priorityScore += 20000L - (long)power[i];
        }
      }

      if (sum > limit)
        continue;

      long utilizationScore = sum * 2L;
      long loadCountScore = 0;

      for (int i = 0; i < 6; i++)
      {
        if (mask & (1 << i))
          loadCountScore += 500L;
      }

      long score = priorityScore + utilizationScore + loadCountScore;

      if (score > bestScore)
      {
        bestScore = score;
        bestMask = mask;
      }
    }

    for (int i = 0; i < 6; i++)
    {
      loadEnabled[i] = true;
      onInverter[i] = (bestMask & (1 << i)) != 0;
    }
  }

  void applyDecision()
  {
    for (int i = 0; i < 6; i++)
    {
      load_relay::setLoadEnabled(i, loadEnabled[i]);
      load_relay::setRelay(i, onInverter[i]);
    }
  }

  void updateFuzzyRisk()
  {
    float safeLoad = fallingMembership(loadRatio, 0.70f, 0.90f);

    float warningLoad = 1.0f - fabs(loadRatio - 0.95f) / 0.25f;
    warningLoad = clampFloat(warningLoad, 0.0f, 1.0f);

    float overloadLoad = risingMembership(loadRatio, 0.95f, 1.10f);

    fuzzyRisk =
        (safeLoad * 0.10f) +
        (warningLoad * 0.55f) +
        (overloadLoad * 1.00f);

    fuzzyRisk = clampFloat(fuzzyRisk, 0.0f, 1.0f);
  }

  void begin()
  {
    setAllLoads(true, true);
    decisionText = "READY";
    applyDecision();
  }

  void update()
  {
    unsigned long now = millis();

    if (now - lastUpdate < updateInterval)
      return;

    lastUpdate = now;

    currentLoad = pzem_sensor::getTotalPower();

    int inverterLimit = config_manager::getInverterPower();
    int margin = config_manager::getLoadMarginPercent();

    effectiveLimit = (inverterLimit * margin) / 100;

    if (effectiveLimit < 1)
      effectiveLimit = 1;

    loadRatio = (float)currentLoad / (float)effectiveLimit;
    updateFuzzyRisk();

    bool nepaAvailable = nepa_sense::isAvailable();
    bool inverterAvailable = inverter_sense::isAvailable();
    bool sensorValid = pzem_sensor::hasValidData();

    if (!nepaAvailable && !inverterAvailable)
    {
      setAllLoads(false, true);
      decisionText = "NO SOURCE";
    }
    else if (!inverterAvailable && nepaAvailable)
    {
      setAllLoads(true, false);
      decisionText = "INVERTER OFF";
    }
    else if (inverterAvailable && !nepaAvailable)
    {
      setAllLoads(true, true);

      if (sensorValid && loadRatio >= 1.0f)
        decisionText = "OVERLOAD NO GRID";
      else
        decisionText = "GRID OFF";
    }
    else if (!sensorValid)
    {
      setAllLoads(true, false);
      decisionText = "PZEM ERROR";
    }
    else if (loadRatio >= 1.0f)
    {
      chooseLoadsForInverter(effectiveLimit);
      decisionText = "SHEDDING TO GRID";
    }
    else if (loadRatio >= 0.90f && fuzzyRisk >= 0.55f)
    {
      chooseLoadsForInverter(effectiveLimit);
      decisionText = "FUZZY OPTIMIZED";
    }
    else
    {
      setAllLoads(true, true);
      decisionText = "NORMAL";
    }

    applyDecision();
  }

  bool isOnInverter(uint8_t relay)
  {
    if (relay >= 6)
      return false;

    return onInverter[relay];
  }

  bool isLoadEnabled(uint8_t relay)
  {
    if (relay >= 6)
      return false;

    return loadEnabled[relay];
  }

  float getLoadRatio()
  {
    return loadRatio;
  }

  float getFuzzyRisk()
  {
    return fuzzyRisk;
  }

  int getCurrentLoad()
  {
    return currentLoad;
  }

  int getEffectiveLimit()
  {
    return effectiveLimit;
  }

  const char *getDecisionText()
  {
    return decisionText;
  }
}
