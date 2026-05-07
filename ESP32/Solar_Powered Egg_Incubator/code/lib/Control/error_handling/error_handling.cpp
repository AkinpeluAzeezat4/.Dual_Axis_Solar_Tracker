#include <Arduino.h>
#include "error_handling.h"
#include "temp_hum.h"
namespace error_handling { static bool errorState=false; void begin(){errorState=false;} void update(){errorState=!temp_hum::isReady();} bool hasError(){return errorState;} }
