#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "gyroscope.h"
#include "Pins.h"

namespace gyroscope
{
    static const uint8_t MPU_ADDR = 0x68;

    static Data data;
    static volatile bool mpuInterruptFlag = false;

    static unsigned long lastRead = 0;
    static unsigned long lastDtTime = 0;
    static const unsigned long updateInterval = 20;

    static float yawAngle = 0.0f;
    static float lastAccelerationMagnitude = 1.0f;

    static const float motionThreshold = 0.20f;
    static const float freeFallThreshold = 0.35f;
    static const float vibrationThreshold = 25.0f;

    static void IRAM_ATTR onMpuInterrupt()
    {
        mpuInterruptFlag = true;
    }

    static bool writeRegister(uint8_t reg, uint8_t value)
    {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(reg);
        Wire.write(value);
        return Wire.endTransmission(true) == 0;
    }

    static bool readRaw(int16_t &ax, int16_t &ay, int16_t &az, int16_t &temp, int16_t &gx, int16_t &gy, int16_t &gz)
    {
        Wire.beginTransmission(MPU_ADDR);
        Wire.write(0x3B);

        if (Wire.endTransmission(false) != 0) return false;
        if (Wire.requestFrom(MPU_ADDR, (uint8_t)14, (uint8_t)true) != 14) return false;

        ax = (Wire.read() << 8) | Wire.read();
        ay = (Wire.read() << 8) | Wire.read();
        az = (Wire.read() << 8) | Wire.read();
        temp = (Wire.read() << 8) | Wire.read();
        gx = (Wire.read() << 8) | Wire.read();
        gy = (Wire.read() << 8) | Wire.read();
        gz = (Wire.read() << 8) | Wire.read();

        return true;
    }

    static void readSensor()
    {
        int16_t rawAx = 0;
        int16_t rawAy = 0;
        int16_t rawAz = 0;
        int16_t rawTemp = 0;
        int16_t rawGx = 0;
        int16_t rawGy = 0;
        int16_t rawGz = 0;

        if (!readRaw(rawAx, rawAy, rawAz, rawTemp, rawGx, rawGy, rawGz))
        {
            data.ready = false;
            return;
        }

        unsigned long now = millis();
        float dt = 0.0f;

        if (lastDtTime > 0)
        {
            dt = (now - lastDtTime) / 1000.0f;
        }

        lastDtTime = now;
        data.ready = true;

        data.ax = rawAx / 16384.0f;
        data.ay = rawAy / 16384.0f;
        data.az = rawAz / 16384.0f;

        data.gx = rawGx / 131.0f;
        data.gy = rawGy / 131.0f;
        data.gz = rawGz / 131.0f;

        data.temperature = (rawTemp / 340.0f) + 36.53f;

        data.pitch = atan2f(data.ay, sqrtf((data.ax * data.ax) + (data.az * data.az))) * 57.2957795f;
        data.roll = atan2f(-data.ax, data.az) * 57.2957795f;

        if (dt > 0.0f && dt < 0.2f)
        {
            yawAngle += data.gz * dt;
        }

        data.yaw = yawAngle;

        data.accelerationMagnitude = sqrtf((data.ax * data.ax) + (data.ay * data.ay) + (data.az * data.az));
        data.gyroMagnitude = sqrtf((data.gx * data.gx) + (data.gy * data.gy) + (data.gz * data.gz));

        if (data.accelerationMagnitude > 0.01f)
        {
            float ratio = data.az / data.accelerationMagnitude;
            ratio = constrain(ratio, -1.0f, 1.0f);
            data.tiltAngle = acosf(ratio) * 57.2957795f;
        }

        data.vibrationLevel = fabsf(data.accelerationMagnitude - lastAccelerationMagnitude);
        lastAccelerationMagnitude = data.accelerationMagnitude;

        data.motionDetected = data.vibrationLevel >= motionThreshold || data.gyroMagnitude >= vibrationThreshold;
        data.freeFallDetected = data.accelerationMagnitude <= freeFallThreshold;

        data.interruptDetected = mpuInterruptFlag;
        mpuInterruptFlag = false;
    }

    void begin()
    {
        Wire.begin(Pins::I2C_SDA, Pins::I2C_SCL);
        Wire.setClock(400000);

        pinMode(Pins::MPU_INT, INPUT);
        attachInterrupt(digitalPinToInterrupt(Pins::MPU_INT), onMpuInterrupt, RISING);

        Wire.beginTransmission(MPU_ADDR);
        data.ready = Wire.endTransmission() == 0;

        if (!data.ready) return;

        bool ok = true;
        ok &= writeRegister(0x6B, 0x00);
        ok &= writeRegister(0x1A, 0x03);
        ok &= writeRegister(0x1B, 0x00);
        ok &= writeRegister(0x1C, 0x00);
        ok &= writeRegister(0x37, 0x00);
        ok &= writeRegister(0x38, 0x01);

        data.ready = ok;

        if (data.ready)
        {
            readSensor();
            lastRead = millis();
        }
    }

    void update()
    {
        if (millis() - lastRead < updateInterval) return;
        lastRead = millis();
        readSensor();
    }

    bool isReady()
    {
        return data.ready;
    }

    bool hasInterrupt()
    {
        bool state = data.interruptDetected;
        data.interruptDetected = false;
        return state;
    }

    Data getData()
    {
        return data;
    }

    void resetYaw()
    {
        yawAngle = 0.0f;
        data.yaw = 0.0f;
    }
}
