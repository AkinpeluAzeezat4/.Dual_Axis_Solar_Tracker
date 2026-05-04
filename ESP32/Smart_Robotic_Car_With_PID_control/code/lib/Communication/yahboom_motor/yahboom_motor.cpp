#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include "yahboom_motor.h"
#include "Pins.h"

namespace yahboom_motor
{
    enum CommandMode
    {
        SPEED_MODE,
        PWM_MODE
    };

    static HardwareSerial motorSerial(2);
    static MotorData data;

    static char rxBuffer[96];
    static char cleanBuffer[96];
    static uint8_t rxIndex = 0;
    static bool receiving = false;

    static unsigned long lastConfigTx = 0;
    static unsigned long lastMotionTx = 0;
    static unsigned long lastUploadTx = 0;
    static uint8_t configIndex = 0;
    static bool pendingMotionTx = true;
    static CommandMode commandMode = SPEED_MODE;

    static const unsigned long configInterval = 140;
    static const unsigned long motionInterval = 60;
    static const unsigned long uploadInterval = 500;

    static const char *configCommands[] = {
        "$mtype:1#",
        "$mphase:40#",
        "$mline:11#",
        "$wdiameter:67.00#",
        "$deadzone:1900#",
        "$upload:3#"};

    static int16_t clampSpeed(int16_t value)
    {
        if (value > 1000) return 1000;
        if (value < -1000) return -1000;
        return value;
    }

    static bool speedsChanged()
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            if (data.targetSpeed[i] != data.commandedSpeed[i]) return true;
        }
        return false;
    }

    static void sendCommand(const char *command)
    {
        motorSerial.print(command);
    }

    static void parseFourLongs(const char *text, long *out)
    {
        char temp[72];
        strncpy(temp, text, sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';

        char *token = strtok(temp, ",");

        for (uint8_t i = 0; i < 4 && token != nullptr; i++)
        {
            out[i] = atol(token);
            token = strtok(nullptr, ",");
        }
    }

    static void parseFourFloats(const char *text, float *out)
    {
        char temp[72];
        strncpy(temp, text, sizeof(temp) - 1);
        temp[sizeof(temp) - 1] = '\0';

        char *token = strtok(temp, ",");

        for (uint8_t i = 0; i < 4 && token != nullptr; i++)
        {
            out[i] = atof(token);
            token = strtok(nullptr, ",");
        }
    }

    static void parseFrame(const char *frame)
    {
        if (strncmp(frame, "MAll:", 5) == 0)
        {
            parseFourLongs(frame + 5, data.encoderTotal);
            data.hasNewFrame = true;
            return;
        }

        if (strncmp(frame, "MTEP:", 5) == 0)
        {
            parseFourLongs(frame + 5, data.encoderOffset);
            data.hasNewFrame = true;
            return;
        }

        if (strncmp(frame, "MSPD:", 5) == 0)
        {
            parseFourFloats(frame + 5, data.measuredSpeed);
            data.hasNewFrame = true;
            return;
        }
    }

    static void readSerial()
    {
        while (motorSerial.available())
        {
            char c = (char)motorSerial.read();

            if (c == '$')
            {
                receiving = true;
                rxIndex = 0;
                memset(rxBuffer, 0, sizeof(rxBuffer));
                continue;
            }

            if (!receiving) continue;

            if (c == '#')
            {
                rxBuffer[rxIndex] = '\0';
                strncpy(cleanBuffer, rxBuffer, sizeof(cleanBuffer) - 1);
                cleanBuffer[sizeof(cleanBuffer) - 1] = '\0';
                receiving = false;
                rxIndex = 0;
                parseFrame(cleanBuffer);
                continue;
            }

            if (rxIndex < sizeof(rxBuffer) - 1)
            {
                rxBuffer[rxIndex++] = c;
            }
            else
            {
                receiving = false;
                rxIndex = 0;
            }
        }
    }

    static void processConfiguration()
    {
        if (data.configured) return;
        if (millis() - lastConfigTx < configInterval) return;

        lastConfigTx = millis();
        sendCommand(configCommands[configIndex]);
        configIndex++;

        if (configIndex >= sizeof(configCommands) / sizeof(configCommands[0]))
        {
            data.configured = true;
            pendingMotionTx = true;
        }
    }

    static void transmitMotion()
    {
        if (!data.configured) return;
        if (!pendingMotionTx && !speedsChanged()) return;
        if (millis() - lastMotionTx < motionInterval) return;

        char command[72];

        if (commandMode == SPEED_MODE)
        {
            snprintf(command, sizeof(command), "$spd:%d,%d,%d,%d#", data.targetSpeed[0], data.targetSpeed[1], data.targetSpeed[2], data.targetSpeed[3]);
        }
        else
        {
            snprintf(command, sizeof(command), "$pwm:%d,%d,%d,%d#", data.targetSpeed[0], data.targetSpeed[1], data.targetSpeed[2], data.targetSpeed[3]);
        }

        sendCommand(command);

        for (uint8_t i = 0; i < 4; i++)
        {
            data.commandedSpeed[i] = data.targetSpeed[i];
        }

        pendingMotionTx = false;
        lastMotionTx = millis();
    }

    static void requestUpload()
    {
        if (!data.configured) return;
        if (millis() - lastUploadTx < uploadInterval) return;
        lastUploadTx = millis();
        sendCommand("$upload:3#");
    }

    void begin()
    {
        motorSerial.begin(115200, SERIAL_8N1, Pins::MOTOR_UART_RX, Pins::MOTOR_UART_TX);
        memset(&data, 0, sizeof(data));
        data.ready = true;
        data.configured = false;
        configIndex = 0;
        lastConfigTx = 0;
        lastMotionTx = 0;
        lastUploadTx = 0;
        pendingMotionTx = true;
        stop();
    }

    void update()
    {
        readSerial();
        processConfiguration();
        transmitMotion();
        requestUpload();
    }

    void setSpeed(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
    {
        commandMode = SPEED_MODE;
        int16_t next[4] = {clampSpeed(m1), clampSpeed(m2), clampSpeed(m3), clampSpeed(m4)};

        for (uint8_t i = 0; i < 4; i++)
        {
            if (next[i] != data.targetSpeed[i]) pendingMotionTx = true;
            data.targetSpeed[i] = next[i];
        }
    }

    void setPWM(int16_t m1, int16_t m2, int16_t m3, int16_t m4)
    {
        commandMode = PWM_MODE;
        int16_t next[4] = {clampSpeed(m1), clampSpeed(m2), clampSpeed(m3), clampSpeed(m4)};

        for (uint8_t i = 0; i < 4; i++)
        {
            if (next[i] != data.targetSpeed[i]) pendingMotionTx = true;
            data.targetSpeed[i] = next[i];
        }
    }

    void stop()
    {
        setSpeed(0, 0, 0, 0);
    }

    void forward(int16_t speed)
    {
        speed = abs(clampSpeed(speed));
        setSpeed(speed, speed, speed, speed);
    }

    void backward(int16_t speed)
    {
        speed = abs(clampSpeed(speed));
        setSpeed(-speed, -speed, -speed, -speed);
    }

    void turnLeft(int16_t speed)
    {
        speed = abs(clampSpeed(speed));
        setSpeed(speed / 3, speed / 3, speed, speed);
    }

    void turnRight(int16_t speed)
    {
        speed = abs(clampSpeed(speed));
        setSpeed(speed, speed, speed / 3, speed / 3);
    }

    void rotateLeft(int16_t speed)
    {
        speed = abs(clampSpeed(speed));
        setSpeed(-speed, -speed, speed, speed);
    }

    void rotateRight(int16_t speed)
    {
        speed = abs(clampSpeed(speed));
        setSpeed(speed, speed, -speed, -speed);
    }

    MotorData getData()
    {
        MotorData copy = data;
        data.hasNewFrame = false;
        return copy;
    }

    bool isReady()
    {
        return data.ready;
    }

    bool isConfigured()
    {
        return data.configured;
    }
}
