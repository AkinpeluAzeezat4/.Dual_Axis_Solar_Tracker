#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <LittleFS.h>
#include "sd_card.h"
#include "Pins.h"
#include "mpu6050_sensor/mpu6050_sensor.h"
#include "battery_level/battery_level.h"
#include "posture_logic/posture_logic.h"

namespace sd_card
{
  static SPIClass sdSPI(VSPI);

  static bool sdReady = false;
  static bool internalReady = false;

  static StorageMode storageMode = STORAGE_NONE;

  static unsigned long lastLog = 0;

  static const unsigned long logInterval = 1000;
  static const char *fileName = "/posture_log.csv";

  static bool fileExists()
  {
    if (storageMode == STORAGE_SD)
    {
      return SD.exists(fileName);
    }

    if (storageMode == STORAGE_INTERNAL)
    {
      return LittleFS.exists(fileName);
    }

    return false;
  }

  static File openFile(const char *path, const char *mode)
  {
    if (storageMode == STORAGE_SD)
    {
      return SD.open(path, mode);
    }

    if (storageMode == STORAGE_INTERNAL)
    {
      return LittleFS.open(path, mode);
    }

    return File();
  }

  static void selectStorage()
  {
    if (sdReady)
    {
      storageMode = STORAGE_SD;
      return;
    }

    if (internalReady)
    {
      storageMode = STORAGE_INTERNAL;
      return;
    }

    storageMode = STORAGE_NONE;
  }

  static void createHeader()
  {
    if (!isReady())
    {
      return;
    }

    if (!fileExists())
    {
      File file = openFile(fileName, FILE_WRITE);

      if (file)
      {
        file.println("time_ms,storage,state,pitch,roll,pitch_error,baseline,battery_voltage,battery_percent,muted");
        file.close();
      }
    }
  }

  void begin()
  {
    sdSPI.begin(Pins::SD_SCK, Pins::SD_MISO, Pins::SD_MOSI, Pins::SD_CS);

    sdReady = SD.begin(Pins::SD_CS, sdSPI);
    internalReady = LittleFS.begin(true);

    selectStorage();
    createHeader();
  }

  void update()
  {
    if (!isReady())
    {
      return;
    }

    unsigned long now = millis();

    if (now - lastLog < logInterval)
    {
      return;
    }

    lastLog = now;
    logNow();
  }

  bool isReady()
  {
    return storageMode != STORAGE_NONE;
  }

  bool isSdReady()
  {
    return sdReady;
  }

  bool isInternalReady()
  {
    return internalReady;
  }

  void logNow()
  {
    if (!isReady())
    {
      return;
    }

    mpu6050_sensor::SensorData data = mpu6050_sensor::getData();

    File file = openFile(fileName, FILE_APPEND);

    if (!file)
    {
      return;
    }

    file.print(millis());
    file.print(',');
    file.print(getStorageName());
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
    if (!isReady())
    {
      return "No storage available";
    }

    File file = openFile(fileName, FILE_READ);

    if (!file)
    {
      return "No log file found";
    }

    String content = "";

    while (file.available())
    {
      content += (char)file.read();

      if (content.length() > 16000)
      {
        content += "\n\nLog preview limited. Use Download CSV for the full file.";
        break;
      }
    }

    file.close();
    return content;
  }

  String readFullLogFile()
  {
    if (!isReady())
    {
      return "No storage available";
    }

    File file = openFile(fileName, FILE_READ);

    if (!file)
    {
      return "No log file found";
    }

    String content = "";

    while (file.available())
    {
      content += (char)file.read();
    }

    file.close();
    return content;
  }

  bool clearLogFile()
  {
    if (!isReady())
    {
      return false;
    }

    if (storageMode == STORAGE_SD)
    {
      if (SD.exists(fileName))
      {
        SD.remove(fileName);
      }
    }
    else if (storageMode == STORAGE_INTERNAL)
    {
      if (LittleFS.exists(fileName))
      {
        LittleFS.remove(fileName);
      }
    }

    createHeader();
    return true;
  }

  const char *getLogFileName()
  {
    return fileName;
  }

  const char *getStorageName()
  {
    if (storageMode == STORAGE_SD)
    {
      return "SD";
    }

    if (storageMode == STORAGE_INTERNAL)
    {
      return "INTERNAL";
    }

    return "NONE";
  }

  StorageMode getStorageMode()
  {
    return storageMode;
  }
}
