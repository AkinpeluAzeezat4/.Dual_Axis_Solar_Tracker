#pragma once

namespace load_relay
{
  void begin();
  void update();

  void setRelay(int relay, bool onInverter);
  void setAllInverter();
  void setAllPHCN();

  bool isOnInverter(int relay);

  bool getRelay1();
  bool getRelay2();
  bool getRelay3();
  bool getRelay4();
  bool getRelay5();
  bool getRelay6();
}
