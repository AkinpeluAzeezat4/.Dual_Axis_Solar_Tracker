#include <Arduino.h>
#include "Pins.h"
#include "gpio_expander/gpio_expander.h"
#include "inverter_sense.h"

namespace inverter_sense
{
  bool available = false;
  bool raw = false;

  void begin()
  {
    if (!gpio_expander::isReady())
      return;

    gpio_expander::pinMode(Pins::EXP_INVERTER_SENSE, INPUT);
    update();
  }

  void update()
  {
    if (!gpio_expander::isReady())
    {
      available = false;
      raw = false;
      return;
    }

    raw = gpio_expander::digitalRead(Pins::EXP_INVERTER_SENSE);

    if (Pins::INVERTER_SENSE_ACTIVE_HIGH)
      available = raw;
    else
      available = !raw;
  }

  bool isAvailable()
  {
    return available;
  }

  bool rawState()
  {
    return raw;
  }
}
