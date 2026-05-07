#include <Arduino.h>
#include "battery_level.h"
namespace battery_level {
static uint8_t batteryPin=255; static float scaleFactor=2.0f, voltage=0; static uint8_t percentage=0; static unsigned long lastReadTime=0;
void begin(uint8_t pin,float scale){batteryPin=pin; scaleFactor=scale; analogReadResolution(12); analogSetPinAttenuation(batteryPin,ADC_11db);}
void update(){if(millis()-lastReadTime<1000)return; lastReadTime=millis(); uint32_t total=0; for(uint8_t i=0;i<10;i++) total+=analogRead(batteryPin); float adcVoltage=((total/10.0f)/4095.0f)*3.3f; voltage=adcVoltage*scaleFactor; float pct=((voltage-3.2f)/(4.2f-3.2f))*100.0f; if(pct<0)pct=0; if(pct>100)pct=100; percentage=(uint8_t)pct;}
float getVoltage(){return voltage;} uint8_t getPercentage(){return percentage;}
}
