#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <LittleFS.h>
#include <FS.h>
#include "Pins.h"
#include "sd_card.h"
#include "temp_sensor/temp_sensor.h"
#include "current_sensor/current_sensor.h"
#include "voltage_sensor/voltage_sensor.h"
#include "vibration_sensor/vibration_sensor.h"
#include "load_relay/load_relay.h"
#include "maintenance_manager/maintenance_manager.h"

namespace sd_card
{
  static SPIClass sdSPI(FSPI);

  static bool sdReady = false;
  static bool internalReady = false;
  static bool lastFaultState = false;
  static maintenance_manager::State lastState = maintenance_manager::STATE_NORMAL;

  static unsigned long lastLog = 0;
  static unsigned long logInterval = 2000;
  static unsigned long lastSdRetry = 0;

  static const char *transformerLogFileName = "/transformer_log.csv";
  static const char *analysisLogFileName = "/analysis_log.csv";
  static const char *eventLogFileName = "/event_log.csv";

  static fs::FS *activeFs()
  {
    if (sdReady)
      return &SD;

    if (internalReady)
      return &LittleFS;

    return nullptr;
  }

  static bool appendLineToFs(fs::FS &fs, const char *path, const String &line)
  {
    File file = fs.open(path, FILE_APPEND);

    if (!file)
      return false;

    file.println(line);
    file.close();
    return true;
  }

  static bool appendLine(const char *path, const String &line)
  {
    if (sdReady)
    {
      if (appendLineToFs(SD, path, line))
        return true;

      sdReady = false;
    }

    if (internalReady)
      return appendLineToFs(LittleFS, path, line);

    return false;
  }

  static void ensureHeader(const char *path, const String &header)
  {
    fs::FS *fs = activeFs();

    if (fs == nullptr)
      return;

    if (!fs->exists(path))
    {
      File file = fs->open(path, FILE_WRITE);

      if (file)
      {
        file.println(header);
        file.close();
      }
    }
  }

  static void ensureHeaders()
  {
    ensureHeader(transformerLogFileName, "uptime_ms,temp_c,temp_valid,current_a,current_adc_vrms,transformer_voltage_v,voltage_adc_vrms,vibration_rms_g,x_g,y_g,z_g,relay_on,fault,risk_score,health_score,level,forecast_min,backend");
    ensureHeader(analysisLogFileName, "uptime_ms,risk_score,health_score,level,worst_metric,forecast_min,temp_rate_c_per_min,current_rate_a_per_min,voltage_rate_v_per_min,vibration_rate_g_per_min,recommendation");
    ensureHeader(eventLogFileName, "uptime_ms,event_type,message,level,risk_score,relay_on,backend");
  }

  static void retrySdCard()
  {
    if (sdReady)
      return;

    if (millis() - lastSdRetry < 10000)
      return;

    lastSdRetry = millis();
    sdReady = SD.begin(Pins::SD_CS, sdSPI);

    if (sdReady)
    {
      ensureHeaders();
      logEvent("STORAGE", "SD card detected. Switched backend to SD card.");
    }
  }

  static String cleanCsvText(String text)
  {
    text.replace("\r", " ");
    text.replace("\n", " ");
    text.replace(",", ";");
    return text;
  }

  void begin()
  {
    internalReady = LittleFS.begin(true);

    sdSPI.begin(Pins::SD_SCK, Pins::SD_MISO, Pins::SD_MOSI, Pins::SD_CS);
    sdReady = SD.begin(Pins::SD_CS, sdSPI);

    ensureHeaders();

    if (sdReady)
      logEvent("BOOT", "Storage backend ready on SD card.");
    else if (internalReady)
      logEvent("BOOT", "SD card not available. Internal LittleFS backend is active.");
  }

  void update()
  {
    retrySdCard();

    maintenance_manager::Snapshot snap = maintenance_manager::getSnapshot();

    if (snap.state != lastState)
    {
      logEvent("STATE", String("Transformer condition changed to ") + maintenance_manager::getLevelText());
      lastState = snap.state;
    }

    bool faultNow = maintenance_manager::isFault();

    if (faultNow != lastFaultState)
    {
      logEvent(faultNow ? "FAULT" : "RECOVERY", faultNow ? "Fault latch active. Transformer relay tripped." : "Fault cleared.");
      lastFaultState = faultNow;
    }

    if (millis() - lastLog >= logInterval)
    {
      lastLog = millis();
      logNow();
    }
  }

  void logNow()
  {
    if (!isStorageReady())
      return;

    ensureHeaders();

    maintenance_manager::Snapshot snap = maintenance_manager::getSnapshot();

    String tempText = snap.tempValid ? String(snap.temperatureC, 2) : "nan";
    String forecastText = snap.forecastMinutes >= 0.0f ? String(snap.forecastMinutes, 1) : "unknown";

    String transformerLine;
    transformerLine.reserve(230);
    transformerLine += String(millis());
    transformerLine += ',';
    transformerLine += tempText;
    transformerLine += ',';
    transformerLine += (snap.tempValid ? "1" : "0");
    transformerLine += ',';
    transformerLine += String(current_sensor::getCurrentA(), 3);
    transformerLine += ',';
    transformerLine += String(current_sensor::getVoltageRMS(), 4);
    transformerLine += ',';
    transformerLine += String(voltage_sensor::getVoltageRMS(), 2);
    transformerLine += ',';
    transformerLine += String(voltage_sensor::getAdcVoltageRMS(), 4);
    transformerLine += ',';
    transformerLine += String(vibration_sensor::getVibrationRMS(), 3);
    transformerLine += ',';
    transformerLine += String(vibration_sensor::getX(), 3);
    transformerLine += ',';
    transformerLine += String(vibration_sensor::getY(), 3);
    transformerLine += ',';
    transformerLine += String(vibration_sensor::getZ(), 3);
    transformerLine += ',';
    transformerLine += (load_relay::isOn() ? "1" : "0");
    transformerLine += ',';
    transformerLine += (maintenance_manager::isFault() ? "1" : "0");
    transformerLine += ',';
    transformerLine += String(maintenance_manager::getRiskScore(), 1);
    transformerLine += ',';
    transformerLine += String(maintenance_manager::getHealthScore(), 1);
    transformerLine += ',';
    transformerLine += maintenance_manager::getLevelText();
    transformerLine += ',';
    transformerLine += forecastText;
    transformerLine += ',';
    transformerLine += getBackendName();

    appendLine(transformerLogFileName, transformerLine);

    String analysisLine;
    analysisLine.reserve(240);
    analysisLine += String(millis());
    analysisLine += ',';
    analysisLine += String(snap.riskScore, 1);
    analysisLine += ',';
    analysisLine += String(snap.healthScore, 1);
    analysisLine += ',';
    analysisLine += maintenance_manager::getLevelText();
    analysisLine += ',';
    analysisLine += maintenance_manager::getWorstMetric();
    analysisLine += ',';
    analysisLine += forecastText;
    analysisLine += ',';
    analysisLine += String(snap.tempRatePerMin, 4);
    analysisLine += ',';
    analysisLine += String(snap.currentRatePerMin, 4);
    analysisLine += ',';
    analysisLine += String(snap.voltageRatePerMin, 4);
    analysisLine += ',';
    analysisLine += String(snap.vibrationRatePerMin, 4);
    analysisLine += ',';
    analysisLine += cleanCsvText(maintenance_manager::getRecommendation());

    appendLine(analysisLogFileName, analysisLine);
  }

  void logEvent(const String &eventType, const String &message)
  {
    if (!isStorageReady())
      return;

    ensureHeader(eventLogFileName, "uptime_ms,event_type,message,level,risk_score,relay_on,backend");

    String line;
    line.reserve(180);
    line += String(millis());
    line += ',';
    line += cleanCsvText(eventType);
    line += ',';
    line += cleanCsvText(message);
    line += ',';
    line += maintenance_manager::getLevelText();
    line += ',';
    line += String(maintenance_manager::getRiskScore(), 1);
    line += ',';
    line += (load_relay::isOn() ? "1" : "0");
    line += ',';
    line += getBackendName();

    appendLine(eventLogFileName, line);
  }

  bool isReady()
  {
    return sdReady;
  }

  bool isInternalReady()
  {
    return internalReady;
  }

  bool isStorageReady()
  {
    return sdReady || internalReady;
  }

  bool usingSdCard()
  {
    return sdReady;
  }

  const char *getBackendName()
  {
    if (sdReady)
      return "SD";

    if (internalReady)
      return "INTERNAL";

    return "NONE";
  }

  const char *getTransformerLogFileName()
  {
    return transformerLogFileName;
  }


  const char *getAnalysisLogFileName()
  {
    return analysisLogFileName;
  }

  const char *getEventLogFileName()
  {
    return eventLogFileName;
  }

  void setLogInterval(unsigned long intervalMs)
  {
    if (intervalMs >= 500)
      logInterval = intervalMs;
  }

  unsigned long getLogInterval()
  {
    return logInterval;
  }

  String readTail(const char *path, size_t maxBytes)
  {
    fs::FS *fs = activeFs();

    if (fs == nullptr || !fs->exists(path))
      return "";

    File file = fs->open(path, FILE_READ);

    if (!file)
      return "";

    size_t size = file.size();
    size_t start = 0;

    if (size > maxBytes)
      start = size - maxBytes;

    file.seek(start);

    if (start > 0)
    {
      while (file.available())
      {
        char c = file.read();
        if (c == '\n')
          break;
      }
    }

    String out;
    out.reserve(min((size_t)maxBytes, (size_t)8192));

    while (file.available())
    {
      out += char(file.read());

      if (out.length() >= maxBytes)
        break;
    }

    file.close();
    return out;
  }

  String readFileLimited(const char *path, size_t maxBytes)
  {
    fs::FS *fs = activeFs();

    if (fs == nullptr || !fs->exists(path))
      return "";

    File file = fs->open(path, FILE_READ);

    if (!file)
      return "";

    String out;
    out.reserve(min((size_t)maxBytes, (size_t)8192));

    while (file.available() && out.length() < maxBytes)
    {
      out += char(file.read());
    }

    file.close();
    return out;
  }

  size_t getFileSize(const char *path)
  {
    fs::FS *fs = activeFs();

    if (fs == nullptr || !fs->exists(path))
      return 0;

    File file = fs->open(path, FILE_READ);

    if (!file)
      return 0;

    size_t size = file.size();
    file.close();
    return size;
  }

  File openRead(const char *path)
  {
    fs::FS *fs = activeFs();

    if (fs == nullptr || !fs->exists(path))
      return File();

    return fs->open(path, FILE_READ);
  }
}
