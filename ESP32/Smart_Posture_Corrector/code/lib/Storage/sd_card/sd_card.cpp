#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "sd_card.h"
#include "Pins.h"
#include "mpu6050_sensor/mpu6050_sensor.h"
#include "battery_level/battery_level.h"
#include "posture_logic/posture_logic.h"

namespace sd_card
{
  static SPIClass sdSPI(VSPI);
  static bool ready = false;
  static unsigned long lastLog = 0;

  static const unsigned long logInterval = 1000;
  static const char *fileName = "/posture_log.csv";

  static void createHeader()
  {
    if (!ready)
      return;

    if (!SD.exists(fileName))
    {
      File file = SD.open(fileName, FILE_WRITE);

      if (file)
      {
        file.println("time_ms,state,pitch,roll,pitch_error,baseline,battery_voltage,battery_percent,muted");
        file.close();
      }
    }
  }

  void begin()
  {
    sdSPI.begin(Pins::SD_SCK, Pins::SD_MISO, Pins::SD_MOSI, Pins::SD_CS);
    ready = SD.begin(Pins::SD_CS, sdSPI);
    createHeader();
  }

  void update()
  {
    if (!ready)
      return;

    unsigned long now = millis();

    if (now - lastLog < logInterval)
      return;

    lastLog = now;
    logNow();
  }

  bool isReady()
  {
    return ready;
  }

  void logNow()
  {
    if (!ready)
      return;

    mpu6050_sensor::SensorData data = mpu6050_sensor::getData();

    File file = SD.open(fileName, FILE_APPEND);

    if (!file)
      return;

    file.print(millis());
    file.print(',');
    file.print(posture_logic::getStateText());
    file.print(',');
    file.print(data.pitch, 2);
    file.print(',');
    file.print(data.roll, 2);
    file.print(',');
    file.print(posture_logic::getPitchError(), 2);
    file.print(',');
    file.print(posture_logic::getBaselinePitch(), 2);
    file.print(',');
    file.print(battery_level::getVoltage(), 2);
    file.print(',');
    file.print(battery_level::getPercentage());
    file.print(',');
    file.println(posture_logic::isMuted() ? 1 : 0);

    file.close();
  }

  String readLogFile()
  {
    if (!ready)
      return "SD card not ready";

    File file = SD.open(fileName, FILE_READ);

    if (!file)
      return "No log file found";

    String content = "";

    while (file.available())
    {
      content += (char)file.read();

      if (content.length() > 12000)
      {
        content += "\nLog preview limited. Use download for full file.";
        break;
      }
    }

    file.close();
    return content;
  }

  bool clearLogFile()
  {
    if (!ready)
      return false;

    if (SD.exists(fileName))
    {
      SD.remove(fileName);
    }

    createHeader();
    return true;
  }

  const char *getLogFileName()
  {
    return fileName;
  }
}
