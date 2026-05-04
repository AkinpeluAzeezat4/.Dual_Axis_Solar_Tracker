#include <Arduino.h>
#include "pid_controller.h"

PIDController::PIDController()
{
    kpValue = 0.0f;
    kiValue = 0.0f;
    kdValue = 0.0f;
    minOutput = -1000.0f;
    maxOutput = 1000.0f;
    integral = 0.0f;
    lastError = 0.0f;
    hasLastError = false;
}

void PIDController::setTunings(float kp, float ki, float kd)
{
    kpValue = kp;
    kiValue = ki;
    kdValue = kd;
}

void PIDController::setOutputLimits(float minimum, float maximum)
{
    minOutput = minimum;
    maxOutput = maximum;
}

void PIDController::reset()
{
    integral = 0.0f;
    lastError = 0.0f;
    hasLastError = false;
}

float PIDController::compute(float error, float dt)
{
    if (dt <= 0.0f) dt = 0.001f;

    integral += error * dt;

    float derivative = 0.0f;
    if (hasLastError)
    {
        derivative = (error - lastError) / dt;
    }

    lastError = error;
    hasLastError = true;

    float output = (kpValue * error) + (kiValue * integral) + (kdValue * derivative);

    if (output > maxOutput)
    {
        output = maxOutput;
        if (integral > 0.0f) integral -= error * dt;
    }

    if (output < minOutput)
    {
        output = minOutput;
        if (integral < 0.0f) integral -= error * dt;
    }

    return output;
}
