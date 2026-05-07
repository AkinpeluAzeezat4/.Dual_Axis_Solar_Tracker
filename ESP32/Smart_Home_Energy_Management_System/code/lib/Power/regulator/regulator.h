#pragma once

namespace regulator
{
  void begin();
  void update();

  void set12VEnabled(bool enabled);
  bool is12VEnabled();
}
