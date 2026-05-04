#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

class PIDController
{
public:
    PIDController();

    void setTunings(float kp, float ki, float kd);
    void setOutputLimits(float minimum, float maximum);
    void reset();
    float compute(float error, float dt);

private:
    float kpValue;
    float kiValue;
    float kdValue;
    float minOutput;
    float maxOutput;
    float integral;
    float lastError;
    bool hasLastError;
};

#endif
