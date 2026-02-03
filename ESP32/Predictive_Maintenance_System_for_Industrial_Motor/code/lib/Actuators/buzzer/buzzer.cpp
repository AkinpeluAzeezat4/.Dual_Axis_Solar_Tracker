#include "buzzer/buzzer.h"
const int BUZZER_PIN = Pins::GPIO15;
namespace buzzer
{

    bool active = false;
    bool beeping = false;
    unsigned long beepStart = 0;
    unsigned long beepDuration = 0;

    void begin()
    {
        pinMode(BUZZER_PIN, OUTPUT); // buzzer pin
        Pins::writePin(BUZZER_PIN, LOW);
    }

    void update()
    {
        if (beeping)
        {
            if (millis() - beepStart >= beepDuration)
            {
                off();
                beeping = false;
            }
        }
    }

    void on()
    {
        active = true;
        Pins::writePin(BUZZER_PIN, HIGH);
    }

    void off()
    {
        active = false;
        Pins::writePin(BUZZER_PIN, LOW);
    }

    void beep(uint16_t duration_ms)
    {
        beepDuration = duration_ms;
        beepStart = millis();
        beeping = true;
        on();
    }

}
