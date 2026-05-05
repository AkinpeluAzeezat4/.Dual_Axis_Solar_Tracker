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

  float loadRatio = 0.0f;
  float fuzzyRisk = 0.0f;

  int currentLoad = 0;
  int effectiveLimit = 0;

  const char *decisionText = "NORMAL";

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

  void chooseLoadsByGeneticSearch(int limit)
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
      long score = priorityScore + utilizationScore;

      if (score > bestScore)
      {
        bestScore = score;
        bestMask = mask;
      }
    }

    for (int i = 0; i < 6; i++)
      onInverter[i] = (bestMask & (1 << i)) != 0;
  }

  void begin()
  {
    load_relay::setAllInverter();

    for (int i = 0; i < 6; i++)
      onInverter[i] = true;

    decisionText = "NORMAL";
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

    float safeLoad = fallingMembership(loadRatio, 0.70f, 0.90f);

    float warningLoad = 1.0f - fabs(loadRatio - 0.95f) / 0.25f;
    warningLoad = clampFloat(warningLoad, 0.0f, 1.0f);

    float overloadLoad = risingMembership(loadRatio, 0.95f, 1.10f);

    fuzzyRisk =
        (safeLoad * 0.10f) +
        (warningLoad * 0.55f) +
        (overloadLoad * 1.00f);

    fuzzyRisk = clampFloat(fuzzyRisk, 0.0f, 1.0f);

    if (!inverter_sense::isAvailable())
    {
      for (int i = 0; i < 6; i++)
        onInverter[i] = false;

      decisionText = "INVERTER OFF";
    }
    else if (loadRatio >= 1.0f && nepa_sense::isAvailable())
    {
      chooseLoadsByGeneticSearch(effectiveLimit);
      decisionText = "SHEDDING TO PHCN";
    }
    else if (loadRatio >= 1.0f && !nepa_sense::isAvailable())
    {
      for (int i = 0; i < 6; i++)
        onInverter[i] = true;

      decisionText = "OVERLOAD NO PHCN";
    }
    else if (loadRatio >= 0.90f && nepa_sense::isAvailable() && fuzzyRisk >= 0.55f)
    {
      chooseLoadsByGeneticSearch(effectiveLimit);
      decisionText = "FUZZY OPTIMIZED";
    }
    else
    {
      for (int i = 0; i < 6; i++)
        onInverter[i] = true;

      decisionText = "NORMAL";
    }

    for (int i = 0; i < 6; i++)
      load_relay::setRelay(i, onInverter[i]);
  }

  bool isOnInverter(uint8_t relay)
  {
    if (relay >= 6)
      return false;

    return onInverter[relay];
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
