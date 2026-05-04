#ifndef GYROSCOPE_H
#define GYROSCOPE_H

namespace gyroscope
{
    struct Data
    {
        bool ready = false;
        bool interruptDetected = false;

        float ax = 0.0f;
        float ay = 0.0f;
        float az = 0.0f;

        float gx = 0.0f;
        float gy = 0.0f;
        float gz = 0.0f;

        float temperature = 0.0f;

        float pitch = 0.0f;
        float roll = 0.0f;
        float yaw = 0.0f;

        float accelerationMagnitude = 0.0f;
        float gyroMagnitude = 0.0f;

        float tiltAngle = 0.0f;
        float vibrationLevel = 0.0f;

        bool motionDetected = false;
        bool freeFallDetected = false;
    };

    void begin();
    void update();

    bool isReady();
    bool hasInterrupt();
    Data getData();
    void resetYaw();
}

#endif
