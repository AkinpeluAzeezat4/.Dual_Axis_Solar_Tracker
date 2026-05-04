#include <Arduino.h>
#include <math.h>
#include <Preferences.h>
#include "posture_logic.h"
#include "mpu6050_sensor/mpu6050_sensor.h"
#include "vibration_motor/vibration_motor.h"
#include "buzzer/buzzer.h"
#include "led_indicator/led_indicator.h"

namespace posture_logic
{
  static Preferences prefs;

  static State state = CALIBRATING;

  static bool muted = false;
  static bool calibrating = false;
  static bool savedCalibration = false;

  static unsigned long calibrationStart = 0;
  static unsigned long badPostureStart = 0;

  static float baselinePitch = 0.0f;
  static float pitchError = 0.0f;
  static float calibrationSum = 0.0f;

  static uint16_t calibrationCount = 0;

  static const unsigned long calibrationTime = 2000;
  static const unsigned long alertDelay = 3000;

  static const float badAngleThreshold = 15.0f;
  static const float goodAngleThreshold = 8.0f;

  static void stopAlerts()
  {
    vibration_motor::setAlert(false);
    buzzer::setAlert(false);
  }

  static void saveCalibration()
  {
    prefs.putFloat("baseline", baselinePitch);
    prefs.putBool("saved", true);
    savedCalibration = true;
  }

  static bool loadCalibration()
  {
    savedCalibration = prefs.getBool("saved", false);

    if (savedCalibration)
    {
      baselinePitch = prefs.getFloat("baseline", 0.0f);
      calibrating = false;
      state = GOOD_POSTURE;
      return true;
    }

    return false;
  }

  static void startCalibration()
  {
    calibrating = true;
    calibrationStart = millis();
    calibrationSum = 0.0f;
    calibrationCount = 0;
    badPostureStart = 0;
    pitchError = 0.0f;
    state = CALIBRATING;

    stopAlerts();
    led_indicator::setFault(false);
  }

  void begin()
  {
    prefs.begin("posture", false);

    if (!loadCalibration())
    {
      startCalibration();
    }
  }

  void update()
  {
    if (!mpu6050_sensor::isReady())
    {
      state = SENSOR_ERROR;
      stopAlerts();
      buzzer::setAlert(true);
      led_indicator::setFault(true);
      return;
    }

    mpu6050_sensor::SensorData data = mpu6050_sensor::getData();
    unsigned long now = millis();

    if (calibrating)
    {
      state = CALIBRATING;
      calibrationSum += data.pitch;
      calibrationCount++;

      if (now - calibrationStart >= calibrationTime && calibrationCount > 0)
      {
        baselinePitch = calibrationSum / calibrationCount;
        saveCalibration();

        calibrating = false;
        state = GOOD_POSTURE;

        vibration_motor::pulse(300);
        buzzer::beep(120);
      }

      return;
    }

    pitchError = data.pitch - baselinePitch;
    float absError = fabsf(pitchError);

    if (absError >= badAngleThreshold)
    {
      if (badPostureStart == 0)
      {
        badPostureStart = now;
      }

      if (now - badPostureStart >= alertDelay)
      {
        state = ALERTING;
        vibration_motor::setAlert(!muted);
        buzzer::setAlert(!muted);
        led_indicator::setFault(true);
      }
      else
      {
        state = BAD_POSTURE;
        stopAlerts();
        led_indicator::setFault(false);
      }
    }
    else if (absError <= goodAngleThreshold)
    {
      badPostureStart = 0;
      state = GOOD_POSTURE;
      stopAlerts();
      led_indicator::setFault(false);
    }
    else
    {
      state = BAD_POSTURE;
      stopAlerts();
      led_indicator::setFault(false);
    }
  }

  void requestCalibration()
  {
    startCalibration();
  }

  void clearSavedCalibration()
  {
    prefs.remove("baseline");
    prefs.putBool("saved", false);
    savedCalibration = false;
    startCalibration();
  }

  void setMuted(bool stateValue)
  {
    muted = stateValue;

    if (muted)
    {
      stopAlerts();
    }
  }

  bool isMuted()
  {
    return muted;
  }

  State getState()
  {
    return state;
  }

  float getBaselinePitch()
  {
    return baselinePitch;
  }

  float getPitchError()
  {
    return pitchError;
  }

  bool hasSavedCalibration()
  {
    return savedCalibration;
  }

  const char *getStateText()
  {
    switch (state)
    {
    case CALIBRATING:
      return "CALIBRATING";
    case GOOD_POSTURE:
      return "GOOD";
    case BAD_POSTURE:
      return "BAD";
    case ALERTING:
      return "ALERT";
    case SENSOR_ERROR:
      return "SENSOR_ERROR";
    default:
      return "UNKNOWN";
    }
  }
}
