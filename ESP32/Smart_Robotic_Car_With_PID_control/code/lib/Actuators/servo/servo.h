#ifndef SERVO_H
#define SERVO_H

namespace servo
{
  void begin();
  void update();
  void setAngle(int angle);
  int getAngle();
  void center();
}

#endif
