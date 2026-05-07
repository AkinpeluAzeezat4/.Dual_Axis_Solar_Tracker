#ifndef ULTRASONIC_H
#define ULTRASONIC_H
#include <Arduino.h>
namespace ultrasonic { void begin(uint8_t trigPin,uint8_t echoPin); void update(); float getDistanceCm(); bool isObjectDetected(float thresholdCm); }
#endif
