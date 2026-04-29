#include <Arduino.h>
#include "buzzer.h"

namespace
{
  uint8_t buzzerPin = 255;
  bool active = false;

  bool singleBeepRunning = false;
  unsigned long singleBeepStart = 0;
  uint16_t singleBeepDuration = 0;

  uint8_t patternLength = 0;
  uint8_t patternIndex = 0;
  unsigned long patternChangedAt = 0;
  uint16_t patternSteps[8] = {0};

  void applyOutput(bool state)
  {
    if (buzzerPin == 255)
      return;

    active = state;
    digitalWrite(buzzerPin, state ? HIGH : LOW);
  }

  void clearPattern()
  {
    patternLength = 0;
    patternIndex = 0;
  }

  void startPattern(const uint16_t *steps, uint8_t length)
  {
    clearPattern();
    singleBeepRunning = false;

    if (steps == nullptr || length == 0)
    {
      applyOutput(false);
      return;
    }

    patternLength = length > 8 ? 8 : length;
    patternIndex = 0;
    patternChangedAt = millis();

    for (uint8_t i = 0; i < patternLength; ++i)
    {
      patternSteps[i] = steps[i];
    }

    applyOutput(true);
  }
}

namespace buzzer
{
  void begin(uint8_t pin)
  {
    buzzerPin = pin;
    pinMode(buzzerPin, OUTPUT);
    applyOutput(false);
  }

  void update()
  {
    if (buzzerPin == 255)
      return;

    if (singleBeepRunning)
    {
      if (millis() - singleBeepStart >= singleBeepDuration)
      {
        singleBeepRunning = false;
        applyOutput(false);
      }
      return;
    }

    if (patternLength == 0)
      return;

    if (millis() - patternChangedAt < patternSteps[patternIndex])
      return;

    patternChangedAt = millis();
    patternIndex++;

    if (patternIndex >= patternLength)
    {
      clearPattern();
      applyOutput(false);
      return;
    }

    applyOutput((patternIndex % 2) == 0);
  }

  void on()
  {
    clearPattern();
    singleBeepRunning = false;
    applyOutput(true);
  }

  void off()
  {
    clearPattern();
    singleBeepRunning = false;
    applyOutput(false);
  }

  void beep(uint16_t durationMs)
  {
    clearPattern();
    singleBeepRunning = true;
    singleBeepStart = millis();
    singleBeepDuration = durationMs;
    applyOutput(true);
  }

  void playSuccess()
  {
    const uint16_t steps[] = {60, 40, 60};
    startPattern(steps, 3);
  }

  void playFailure()
  {
    const uint16_t steps[] = {180, 70, 180};
    startPattern(steps, 3);
  }

  void playRegister()
  {
    const uint16_t steps[] = {70, 40, 70, 40, 70};
    startPattern(steps, 5);
  }

  bool isActive()
  {
    return active;
  }
}
