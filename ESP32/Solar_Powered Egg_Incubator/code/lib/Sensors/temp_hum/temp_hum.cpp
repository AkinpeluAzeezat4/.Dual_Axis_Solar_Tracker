#include <Arduino.h>
#include <Adafruit_AHTX0.h>
#include "temp_hum.h"
namespace temp_hum {
static Adafruit_AHTX0 aht; static float temperature=0,humidity=0; static bool ready=false; static unsigned long lastRead=0;
void begin(){ready=aht.begin();}
void update(){if(!ready)return; if(millis()-lastRead<2000)return; lastRead=millis(); sensors_event_t humEvent,tempEvent; aht.getEvent(&humEvent,&tempEvent); temperature=tempEvent.temperature; humidity=humEvent.relative_humidity;}
float getTemperature(){return temperature;} float getHumidity(){return humidity;} bool isReady(){return ready;}
}
