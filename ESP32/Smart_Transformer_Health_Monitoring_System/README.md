# Smart Transformer Health Monitoring System

## Overview

This project is a modular Smart Transformer Health Monitoring System Monitoring built around the ESP32-S3 WROOM-1U using a structured PlatformIO architecture.

The system continuously monitors:
- Transformer vibration
- Transformer current consumption
- Transformer temperature

The ESP32-S3 processes sensor data locally to detect:
- mechanical wear
- overload conditions
- overheating
- excessive vibration
- possible Transformer failure

The system includes:
- 20x4 I2C LCD monitoring
- SD card logging
- automatic relay shutdown
- buzzer alarm
- breathing status LED
- rotary encoder navigation

---

# Features

- ESP32-S3 WROOM-1U based system
- ADXL345 vibration monitoring
- ZMCT103C current monitoring
- DS18B20 temperature monitoring
- 20x4 I2C LCD display
- SD card CSV logging
- Relay protection
- Fault detection system
- Rotary encoder navigation
- Breathing LED indication
- Modular PlatformIO structure
- Non-blocking firmware design

---

# Hardware Used

## Main Controller
- ESP32-S3 WROOM-1U

## Display
- 20x4 I2C LCD

## Sensors
- ADXL345 vibration sensor
- ZMCT103C current sensor
- DS18B20 temperature sensor

## Storage
- MicroSD card module

## User Input
- Rotary encoder

## Output Devices
- Relay module
- Active buzzer
- Onboard LED

---

# Pin Configuration

## I2C Bus

| Function | GPIO |
|---|---|
| LCD SDA | GPIO8 |
| LCD SCL | GPIO9 |
| ADXL345 SDA | GPIO8 |
| ADXL345 SCL | GPIO9 |

---

## Sensors

| Function | GPIO |
|---|---|
| ADXL345 INT | GPIO10 |
| ZMCT103C ADC | GPIO1 |
| DS18B20 DATA | GPIO4 |

---

## SD Card SPI

| Function | GPIO |
|---|---|
| SD CS | GPIO5 |
| SD MOSI | GPIO11 |
| SD SCK | GPIO12 |
| SD MISO | GPIO13 |

---

## Rotary Encoder

| Function | GPIO |
|---|---|
| CLK | GPIO16 |
| DT | GPIO17 |
| SW | GPIO18 |

---

## Outputs

| Function | GPIO |
|---|---|
| Relay | GPIO21 |
| Buzzer | GPIO14 |
| Status LED | GPIO48 |

---

# Protection Thresholds

| Parameter | Threshold |
|---|---|
| Current | 5.0 A |
| Temperature | 70 °C |
| Vibration RMS | 2.5 g |

---

# SD Card Logging

Example log file:

```csv
millis,temp_c,current_a,vibration_rms_g,relay_on,fault
1200,32.4,1.22,0.11,1,0
5200,33.1,1.35,0.10,1,0
10200,71.0,2.80,3.10,0,1
```

---

# LED Behaviour

## Normal
- breathing LED effect

## Fault
- fast blinking LED

---

# Relay Protection

The relay disconnects the Transformer automatically when:
- current exceeds threshold
- temperature exceeds threshold
- vibration exceeds threshold

---

# Project Structure

```text
lib/
├── Actuators/
│   └── buzzer/
├── Communication/
│   └── wifi_manager/
├── Control/
│   ├── error_handling/
│   ├── load_relay/
│   └── sleep_wake/
├── Display/
│   ├── lcd_screen/
│   └── led_indicator/
├── Input/
│   └── rotary_encoder/
├── Pins/
├── Sensors/
│   ├── current_sensor/
│   ├── temp_sensor/
│   └── vibration_sensor/
├── Storage/
│   └── sd_card/
├── System/
│   └── reset/

src/
└── main.cpp
```

---

# PlatformIO Environment

```ini
[env:esp32-s3-devkitm-1]
platform = espressif32
board = esp32-s3-devkitm-1
framework = arduino
monitor_speed = 115200
```

---

# Build and Upload

## Build

```bash
pio run
```

## Upload

```bash
pio run --target upload
```

## Serial Monitor

```bash
pio device monitor
```

---

# Important Notes

- Use a proper conditioning circuit for the ZMCT103C
- DS18B20 requires a 4.7k pull-up resistor
- Ensure relay module is 3.3V compatible
- Use proper isolation when working with industrial Transformers
- The system uses non-blocking firmware architecture

---

# Future Improvements

- Wi-Fi dashboard
- OTA update support
- Cloud logging
- Machine learning prediction
- MQTT integration
- Historical graph analysis

---

# Author

Egbe Raymond

Electrical / Electronic Engineer  
Embedded Systems Developer  
Full Stack Developer

---

# License

This project is intended for educational, industrial prototype, and development purposes.
