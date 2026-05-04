#include "Pins.h"

namespace Pins
{
    const uint8_t MOTOR_UART_RX = 16;
    const uint8_t MOTOR_UART_TX = 17;

    const uint8_t I2C_SDA = 21;
    const uint8_t I2C_SCL = 22;
    const uint8_t MPU_INT = 35;

    const uint8_t ULTRASONIC_TRIG = 27;
    const uint8_t ULTRASONIC_ECHO = 26;
    const uint8_t SERVO_SIGNAL = 25;

    const uint8_t BATTERY_ADC = 34;
    const uint8_t POWER_LED = 2;
    const uint8_t BATTERY_LED1 = 13;
    const uint8_t BATTERY_LED2 = 14;
    const uint8_t BATTERY_LED3 = 18;
    const uint8_t BATTERY_LED4 = 19;

    void begin()
    {
        pinMode(ULTRASONIC_TRIG, OUTPUT);
        pinMode(ULTRASONIC_ECHO, INPUT);
        pinMode(MPU_INT, INPUT);
        pinMode(BATTERY_ADC, INPUT);

        pinMode(POWER_LED, OUTPUT);
        pinMode(BATTERY_LED1, OUTPUT);
        pinMode(BATTERY_LED2, OUTPUT);
        pinMode(BATTERY_LED3, OUTPUT);
        pinMode(BATTERY_LED4, OUTPUT);

        digitalWrite(ULTRASONIC_TRIG, LOW);
        digitalWrite(POWER_LED, LOW);
        digitalWrite(BATTERY_LED1, LOW);
        digitalWrite(BATTERY_LED2, LOW);
        digitalWrite(BATTERY_LED3, LOW);
        digitalWrite(BATTERY_LED4, LOW);
    }

    void update()
    {
    }

    int readPin(uint8_t gpio)
    {
        return digitalRead(gpio);
    }

    int readAnalog(uint8_t gpio)
    {
        return analogRead(gpio);
    }

    void writePin(uint8_t gpio, bool value)
    {
        digitalWrite(gpio, value ? HIGH : LOW);
    }
}
