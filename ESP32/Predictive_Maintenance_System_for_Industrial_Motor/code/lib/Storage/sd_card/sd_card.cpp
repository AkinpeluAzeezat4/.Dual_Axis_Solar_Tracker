#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "Pins.h"
#include "sd_card.h"
#include "temp_sensor/temp_sensor.h"
#include "current_sensor/current_sensor.h"
#include "vibration_sensor/vibration_sensor.h"
#include "load_relay/load_relay.h"

namespace sd_card
{
  static SPIClass sdSPI(FSPI);

  static bool ready = false;

  static unsigned long lastLog = 0;
  static unsigned long logInterval = 5000;

  static const char *fileName = "/motor_log.csv";

  static void createHeader()
  {
    if (!ready)
    {
      return;
    }

    if (!SD.exists(fileName))
    {
      File file = SD.open(fileName, FILE_WRITE);

      if (file)
      {
        file.println("millis,temp_c,current_a,vibration_rms_g,relay_on,fault");
        file.close();
      }
    }
  }

  void begin()
  {
    sdSPI.begin(Pins::SD_SCK, Pins::SD_MISO, Pins::SD_MOSI, Pins::SD_CS);

    ready = SD.begin(Pins::SD_CS, sdSPI);

    if (ready)
    {
      createHeader();
    }
  }

  void update()
  {
    if (!ready)
    {
      return;
    }

    if (millis() - lastLog >= logInterval)
    {
      lastLog = millis();
      logNow();
    }
  }

  void logNow()
  {
    if (!ready)
    {
      return;
    }

    File file = SD.open(fileName, FILE_APPEND);

    if (!file)
    {
      return;
    }

    file.print(millis());
    file.print(',');

    if (temp_sensor::isValid())
    {
      file.print(temp_sensor::getTemperatureC(), 2);
    }
    else
    {
      file.print("nan");
    }

    file.print(',');
    file.print(current_sensor::getCurrentA(), 3);
    file.print(',');
    file.print(vibration_sensor::getVibrationRMS(), 3);
    file.print(',');
    file.print(load_relay::isOn() ? 1 : 0);
    file.print(',');
    file.println(load_relay::isFault() ? 1 : 0);

    file.close();
  }

  bool isReady()
  {
    return ready;
  }

  void setLogInterval(unsigned long intervalMs)
  {
    if (intervalMs >= 1000)
    {
      logInterval = intervalMs;
    }
  }

  const char *getFileName()
  {
    return fileName;
  }
}
