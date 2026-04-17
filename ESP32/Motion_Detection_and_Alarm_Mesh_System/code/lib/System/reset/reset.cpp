#include "reset/reset.h"

namespace reset
{

    void begin()
    {
        // nothing needed
    }

    void trigger()
    {
        ESP.restart();
    }

}
