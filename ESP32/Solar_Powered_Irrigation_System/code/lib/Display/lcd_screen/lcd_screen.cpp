#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "lcd_screen.h"
#include "Pins.h"
#include "temp_hum/temp_hum.h"
#include "ultrasonic/ultrasonic.h"
#include "soil_sensor/soil_sensor.h"
#include "battery_level/battery_level.h"
#include "solar_level/solar_level.h"
#include "pump/pump.h"

namespace lcd_screen
{
    static LiquidCrystal_I2C lcd(0x27, 20, 4);

    static unsigned long lastUpdate = 0;
    static const unsigned long interval = 1000;

    static void printLine(uint8_t row, const String &text)
    {
        String line = text;

        if (line.length() > 20)
        {
            line = line.substring(0, 20);
        }

        while (line.length() < 20)
        {
            line += " ";
        }

        lcd.setCursor(0, row);
        lcd.print(line);
    }

    void begin()
    {
        Wire.begin(Pins::OLED_SDA, Pins::OLED_SCL);

        lcd.init();
        lcd.backlight();
        lcd.clear();

        printLine(0, "Solar Irrigation");
        printLine(1, "System Starting...");
        printLine(2, "ESP32-WROOM-32UE");
        printLine(3, "Please Wait");
    }

    void update()
    {
        if (millis() - lastUpdate < interval) return;

        lastUpdate = millis();

        char line[24];

        snprintf(line, sizeof(line), "Tank:%3d%% Soil:%3d%%",
                 ultrasonic::getLevelPercent(),
                 soil_sensor::getMoisturePercent());
        printLine(0, line);

        snprintf(line, sizeof(line), "T:%4.1fC H:%3.0f%%",
                 temp_hum::getTemperature(),
                 temp_hum::getHumidity());
        printLine(1, line);

        snprintf(line, sizeof(line), "Bat:%4.2fV Sol:%4.2f",
                 battery_level::getVoltage(),
                 solar_level::getVoltage());
        printLine(2, line);

        snprintf(line, sizeof(line), "TankP:%s Irr:%s",
                 pump::isTankPumpOn() ? "ON " : "OFF",
                 pump::isIrrigationPumpOn() ? "ON " : "OFF");
        printLine(3, line);
    }

    void showMessage(const char *line1, const char *line2, const char *line3, const char *line4)
    {
        printLine(0, line1);
        printLine(1, line2);
        printLine(2, line3);
        printLine(3, line4);
    }
}
