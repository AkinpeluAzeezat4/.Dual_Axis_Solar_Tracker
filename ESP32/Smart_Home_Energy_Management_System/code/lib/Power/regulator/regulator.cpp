#include <Arduino.h>
#include "Pins.h"
#include "gpio_expander/gpio_expander.h"
#include "regulator.h"

namespace regulator
{
  bool enabled12V = true;

  void apply()
  {
    if (!gpio_expander::isReady())
      return;

    gpio_expander::pinMode(Pins::EXP_12V_EN, OUTPUT);
    gpio_expander::digitalWrite(Pins::EXP_12V_EN, enabled12V);
  }

  void begin()
  {
    enabled12V = true;
    apply();
  }

  void update()
  {
  }

  void set12VEnabled(bool enabled)
  {
    enabled12V = enabled;
    apply();
  }

  bool is12VEnabled()
  {
    return enabled12V;
  }
}
