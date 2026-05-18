#include <Arduino.h>
#include "Pins.h"
#include "gpio_expander/gpio_expander.h"
#include "nepa_sense.h"

namespace nepa_sense
{
  bool available = false;
  bool raw = false;

  void begin()
  {
    if (!gpio_expander::isReady())
      return;

    gpio_expander::pinMode(Pins::EXP_NEPA_SENSE, INPUT);
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

    raw = gpio_expander::digitalRead(Pins::EXP_NEPA_SENSE);

    if (Pins::SOURCE_SENSE_ACTIVE_HIGH)
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
