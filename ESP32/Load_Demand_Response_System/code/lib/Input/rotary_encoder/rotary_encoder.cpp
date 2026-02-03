#include "rotary_encoder.h"

const int ENCODER_A = Pins::GPIO15;
const int ENCODER_B = Pins::GPIO15;
const int ENCODER_SW = Pins::GPIO15;

namespace rotary_encoder
{

    // Encoder state
    volatile int position = 0;
    volatile bool lastA = 0;
    volatile bool lastB = 0;
    bool buttonState = false;

    void begin()
    {
        lastA = Pins::readPin(ENCODER_A); // encoder A
        lastB = Pins::readPin(ENCODER_B); // encoder B
    }

    void update()
    {
        bool A = Pins::readPin(ENCODER_A); // encoder A
        bool B = Pins::readPin(ENCODER_B); // encoder B

        // Detect rotation
        if (A != lastA)
        {
            if (A == B)
                position++;
            else
                position--;
        }
        lastA = A;
        lastB = B;

        // Button state
        buttonState = (Pins::readPin(ENCODER_SW) == LOW); // encoder button
    }

    int getPosition() { return position; }
    bool isPressed() { return buttonState; }

}
