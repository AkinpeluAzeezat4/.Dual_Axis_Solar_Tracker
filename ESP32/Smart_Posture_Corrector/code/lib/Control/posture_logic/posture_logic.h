#pragma once
#include <Arduino.h>

namespace posture_logic
{
  enum State
  {
    CALIBRATING,
    GOOD_POSTURE,
    BAD_POSTURE,
    ALERTING,
    SENSOR_ERROR
  };

  void begin();
  void update();

  void requestCalibration();
  void clearSavedCalibration();

  void setMuted(bool stateValue);
  bool isMuted();

  State getState();

  float getBaselinePitch();
  float getPitchError();
  float getCurrentPitch();
  float getIdealReferenceAngle();

  bool hasSavedCalibration();
  bool isCalibrationValid();

  const char *getStateText();
}
