#include "motor_driver.h"
#include "pins.h"
#include "sun_sensor/sun_sensor.h"
#include "rotary_encoder/rotary_encoder.h"

namespace motor_driver
{
  Mode mode = AUTO_TRACK;

  int azSpeed = 0;
  int elSpeed = 0;
  int manualSpeed = 160;

  bool leftState = false;
  bool rightState = false;
  bool upState = false;
  bool downState = false;

  unsigned long lastMoveTime = 0;

  const int azChannel = 0;
  const int elChannel = 1;
  const int pwmFreq = 18000;
  const int pwmResolution = 8;

  const int minSpeed = 120;
  const int maxSpeed = 230;
  const unsigned long manualPulseTime = 250;

  int clampValue(int value, int low, int high)
  {
    if (value < low)
      return low;

    if (value > high)
      return high;

    return value;
  }

  void azimuth(int speed)
  {
    speed = clampValue(speed, -255, 255);

    leftState = speed < 0;
    rightState = speed > 0;

    if (speed > 0)
    {
      digitalWrite(Pins::AZ_IN1, HIGH);
      digitalWrite(Pins::AZ_IN2, LOW);
      ledcWrite(azChannel, speed);
    }
    else if (speed < 0)
    {
      digitalWrite(Pins::AZ_IN1, LOW);
      digitalWrite(Pins::AZ_IN2, HIGH);
      ledcWrite(azChannel, -speed);
    }
    else
    {
      digitalWrite(Pins::AZ_IN1, LOW);
      digitalWrite(Pins::AZ_IN2, LOW);
      ledcWrite(azChannel, 0);
    }
  }

  void elevation(int speed)
  {
    speed = clampValue(speed, -255, 255);

    upState = speed > 0;
    downState = speed < 0;

    if (speed > 0)
    {
      digitalWrite(Pins::EL_IN1, HIGH);
      digitalWrite(Pins::EL_IN2, LOW);
      ledcWrite(elChannel, speed);
    }
    else if (speed < 0)
    {
      digitalWrite(Pins::EL_IN1, LOW);
      digitalWrite(Pins::EL_IN2, HIGH);
      ledcWrite(elChannel, -speed);
    }
    else
    {
      digitalWrite(Pins::EL_IN1, LOW);
      digitalWrite(Pins::EL_IN2, LOW);
      ledcWrite(elChannel, 0);
    }
  }

  void cycleMode()
  {
    if (mode == AUTO_TRACK)
      mode = MANUAL_AZIMUTH;
    else if (mode == MANUAL_AZIMUTH)
      mode = MANUAL_ELEVATION;
    else if (mode == MANUAL_ELEVATION)
      mode = CALIBRATE_DEADBAND;
    else if (mode == CALIBRATE_DEADBAND)
      mode = CALIBRATE_NIGHT;
    else
      mode = AUTO_TRACK;
  }

  void begin()
  {
    pinMode(Pins::AZ_IN1, OUTPUT);
    pinMode(Pins::AZ_IN2, OUTPUT);
    pinMode(Pins::EL_IN1, OUTPUT);
    pinMode(Pins::EL_IN2, OUTPUT);

    ledcSetup(azChannel, pwmFreq, pwmResolution);
    ledcSetup(elChannel, pwmFreq, pwmResolution);

    ledcAttachPin(Pins::AZ_EN, azChannel);
    ledcAttachPin(Pins::EL_EN, elChannel);

    stopAll();
  }

  void update()
  {
    rotary_encoder::ButtonEvent event = rotary_encoder::getButtonEvent();

    if (event == rotary_encoder::SHORT_PRESS)
    {
      stopAll();
      cycleMode();
      rotary_encoder::clearButtonEvent();
    }
    else if (event == rotary_encoder::LONG_PRESS)
    {
      mode = AUTO_TRACK;
      stopAll();
      rotary_encoder::clearButtonEvent();
    }

    int delta = rotary_encoder::getDelta();
    sun_sensor::SensorData data = sun_sensor::getData();

    if (data.isDark && mode == AUTO_TRACK)
    {
      stopAll();
      return;
    }

    if (mode == AUTO_TRACK)
    {
      int deadband = sun_sensor::getDeadband();
      int h = data.horizontalError;
      int v = data.verticalError;

      if (abs(h) > deadband)
      {
        int mappedSpeed = map(clampValue(abs(h), deadband, 2500), deadband, 2500, minSpeed, maxSpeed);
        azSpeed = mappedSpeed * (h > 0 ? -1 : 1);
      }
      else
      {
        azSpeed = 0;
      }

      if (abs(v) > deadband)
      {
        int mappedSpeed = map(clampValue(abs(v), deadband, 2500), deadband, 2500, minSpeed, maxSpeed);
        elSpeed = mappedSpeed * (v > 0 ? 1 : -1);
      }
      else
      {
        elSpeed = 0;
      }

      azimuth(azSpeed);
      elevation(elSpeed);
      return;
    }

    if (mode == MANUAL_AZIMUTH)
    {
      elevation(0);

      if (delta != 0)
      {
        azimuth(delta > 0 ? manualSpeed : -manualSpeed);
        lastMoveTime = millis();
      }

      if (millis() - lastMoveTime > manualPulseTime)
        azimuth(0);

      return;
    }

    if (mode == MANUAL_ELEVATION)
    {
      azimuth(0);

      if (delta != 0)
      {
        elevation(delta > 0 ? manualSpeed : -manualSpeed);
        lastMoveTime = millis();
      }

      if (millis() - lastMoveTime > manualPulseTime)
        elevation(0);

      return;
    }

    if (mode == CALIBRATE_DEADBAND)
    {
      stopAll();

      if (delta != 0)
        sun_sensor::setDeadbandOffset(sun_sensor::getDeadbandOffset() + (delta * 10));

      return;
    }

    if (mode == CALIBRATE_NIGHT)
    {
      stopAll();

      if (delta != 0)
        sun_sensor::setNightThreshold(sun_sensor::getNightThreshold() + (delta * 20));

      return;
    }
  }

  void stopAll()
  {
    azimuth(0);
    elevation(0);
  }

  Mode getMode()
  {
    return mode;
  }

  const char *getModeName()
  {
    switch (mode)
    {
    case AUTO_TRACK:
      return "AUTO";

    case MANUAL_AZIMUTH:
      return "MANUAL_AZ";

    case MANUAL_ELEVATION:
      return "MANUAL_EL";

    case CALIBRATE_DEADBAND:
      return "CAL_DEADBAND";

    case CALIBRATE_NIGHT:
      return "CAL_NIGHT";

    default:
      return "UNKNOWN";
    }
  }

  bool isMovingLeft()
  {
    return leftState;
  }

  bool isMovingRight()
  {
    return rightState;
  }

  bool isMovingUp()
  {
    return upState;
  }

  bool isMovingDown()
  {
    return downState;
  }
}
