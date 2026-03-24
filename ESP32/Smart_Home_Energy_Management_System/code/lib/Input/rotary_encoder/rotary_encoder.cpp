// #include "rotary_encoder.h"
// // #include "Pins.h"
// const int ENCODER_A = 26;
// const int ENCODER_B = 25;
// const int ENCODER_SW = 27;

// volatile int encoderPos = 0; // raw position
// int stepPosition = 0;        // detent steps
// int direction = 0;

// bool lastButtonState = HIGH;
// int lastEncoderState = 0;
// bool buttonState;
// namespace rotary_encoder
// {

//   void begin()
//   {
//     pinMode(ENCODER_A, INPUT);
//     pinMode(ENCODER_B, INPUT);
//     pinMode(ENCODER_SW, INPUT_PULLUP);

//     // Initial encoder state
//     lastEncoderState = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);
//   }

//   void update()
//   {
//     int state = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);

//     // Combine previous and current state to detect direction
//     int combined = (lastEncoderState << 2) | state;

//     switch (combined)
//     {
//     // Forward steps
//     case 0b0001:
//     case 0b0111:
//     case 0b1110:
//     case 0b1000:
//       encoderPos++;
//       break;

//     // Backward steps
//     case 0b0010:
//     case 0b0100:
//     case 0b1101:
//     case 0b1011:
//       encoderPos--;
//       break;

//     // Ignore other transitions (bouncing)
//     default:
//       break;
//     }

//     lastEncoderState = state;

//     // Convert raw pulses to detent steps
//     int newStep = encoderPos / 4;

//     if (newStep != stepPosition)
//     {
//       Serial.println("direction: inside the logic");
//       if (newStep > stepPosition)
//       {

//         direction = 2;
//         Serial.println("forward first");
//       }
//       else
//       {

//         direction = 1;
//         Serial.println("backward first");
//       }
//       stepPosition = newStep;
//       direction = 0;
//     }

//     // Read button press
//     buttonState = digitalRead(ENCODER_SW);
//     if (buttonState == LOW && lastButtonState == HIGH)
//     {
//       Serial.println("Button Pressed");
//       delay(200); // simple debounce
//     }
//     lastButtonState = buttonState;

//     delay(10); // tiny delay to stabilize readings
//   }
//   // bool isPressed() { return buttonState; }

//   // int getPosition() { return direction; }

// }

// #include <Arduino.h>
// namespace rotary_encoder
// {

//   const int ENCODER_A = 26;  // GPIO25
//   const int ENCODER_B = 25;  // GPIO26
//   const int ENCODER_SW = 27; // GPIO27

//   volatile int encoderPos = 0; // raw position
//   int stepPosition = 0;        // detent steps

//   bool lastButtonState = HIGH;
//   int lastEncoderState = 0;

//   void begin()
//   {
//     pinMode(ENCODER_A, INPUT_PULLUP);
//     pinMode(ENCODER_B, INPUT_PULLUP);
//     pinMode(ENCODER_SW, INPUT_PULLUP);

//     // Initial encoder state
//     lastEncoderState = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);
//   }

//   void update()
//   {
//     // Read encoder pins
//     int state = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);

//     // Combine previous and current state to detect direction
//     int combined = (lastEncoderState << 2) | state;

//     switch (combined)
//     {
//     // Forward steps
//     case 0b0001:
//     case 0b0111:
//     case 0b1110:
//     case 0b1000:
//       encoderPos++;
//       break;

//     // Backward steps
//     case 0b0010:
//     case 0b0100:
//     case 0b1101:
//     case 0b1011:
//       encoderPos--;
//       break;

//     // Ignore other transitions (bouncing)
//     default:
//       break;
//     }

//     lastEncoderState = state;

//     // Convert raw pulses to detent steps
//     int newStep = encoderPos / 4; // each detent is 4 pulses, adjust if needed
//     if (newStep != stepPosition)
//     {
//       if (newStep > stepPosition)
//         Serial.println("Step Forward");
//       else
//         Serial.println("Step Backward");

//       stepPosition = newStep;
//     }
//   }
//   bool isPressed()
//   {

//     bool buttonState = digitalRead(ENCODER_SW);
//     if (buttonState == LOW && lastButtonState == HIGH)
//     {
//       Serial.println("Button Pressed");
//       delay(200); // simple debounce
//     }
//     lastButtonState = buttonState;
//     return buttonState;
//   }
// }

#include "rotary_encoder.h"

const int ENCODER_SW = 27;
const int ENCODER_A = 26;
const int ENCODER_B = 25;

// namespace rotary_encoder
// {

//   volatile int position = 0;
//   volatile int lastEncoded = 0;
//   bool buttonState = false;
//   bool lastButtonState = HIGH; // assuming pull-up
//   int lastEncoderState = 0;
//   void begin()
//   {
//     pinMode(ENCODER_A, INPUT);
//     pinMode(ENCODER_B, INPUT);
//     pinMode(ENCODER_SW, INPUT_PULLUP);

//     lastEncoderState = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);
//   }

//   void update()
//   {
//     // int state = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);
//     // int combined = (lastEncoderState << 2) | state;

//     // switch (combined)
//     // {
//     // case 0b0001:
//     // case 0b0111:
//     // case 0b1110:
//     // case 0b1000:
//     //   encoderPos++;
//     //   break;

//     // case 0b0010:
//     // case 0b0100:
//     // case 0b1101:
//     // case 0b1011:
//     //   encoderPos--;
//     //   break;
//     // }

//     // lastEncoderState = state;

//     // int newStep = encoderPos / 4;

//     // if (newStep != stepPosition)
//     // {
//     //   int newDir = (newStep > stepPosition) ? 1 : -1;

//     //   if (newDir == stableDirection)
//     //   {
//     //     directionCounter++;
//     //   }
//     //   else
//     //   {
//     //     stableDirection = newDir;
//     //     directionCounter = 1;
//     //   }

//     //   // if (directionCounter >= directionThreshold)
//     //   // {
//     //   //   direction = stableDirection;
//     //   //   stepPosition = newStep;
//     //   //   directionCounter = 0;
//     //   // }

//     //   static unsigned long lastMoveTime = 0;
//     //   unsigned long now = millis();
//     //   unsigned long delta = now - lastMoveTime;

//     //   int acceleration = 1;

//     //   // 🔥 Fast turn detection
//     //   if (delta < 40) // very fast
//     //     acceleration = 4;
//     //   else if (delta < 80) // medium
//     //     acceleration = 2;
//     //   else
//     //     acceleration = 1;

//     //   direction = stableDirection * acceleration;

//     //   stepPosition = newStep;
//     //   directionCounter = 0;
//     //   lastMoveTime = now;
//     // }

//     // bool buttonState = digitalRead(ENCODER_SW);
//     // if (buttonState == LOW && lastButtonState == HIGH)
//     //   buttonPressed = true;

//     // lastButtonState = buttonState;

//     int MSB = digitalRead(ENCODER_A);
//     int LSB = digitalRead(ENCODER_B);

//     int encoded = (MSB << 1) | LSB;
//     int sum = (lastEncoded << 2) | encoded;

//     if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
//       position++;
//     if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
//       position--;

//     lastEncoded = encoded;

//     // read button state
//     buttonState = (digitalRead(ENCODER_SW) == LOW); // LOW = pressed
//   }

//   int getDirection()
//   {
//     int dir = position;
//     position = 0;
//     return dir;
//   }
//   // bool isPressed()
//   // {
//   //   return buttonState; // stays true as long as button is physically pressed
//   // }
//   // bool wasPressed()
//   // {
//   //   bool pressedNow = (digitalRead(ENCODER_SW) == LOW);
//   //   // delay(200);
//   //   bool wasPressed = false;

//   //   if (pressedNow && lastButtonState == HIGH)
//   //   {
//   //     wasPressed = true; // button transitioned
//   //   }
//   //   lastButtonState = pressedNow;
//   //   return wasPressed;
//   // }
//   bool wasPressed()
//   {
//     static bool lastReading = HIGH;
//     static unsigned long lastDebounceTime = 0;
//     const unsigned long debounceDelay = 50;

//     bool reading = digitalRead(ENCODER_SW);
//     bool event = false;
//     unsigned long now = millis();

//     if (reading != lastReading)
//     {
//       lastDebounceTime = now;
//     }

//     if ((now - lastDebounceTime) > debounceDelay)
//     {
//       if (reading == LOW && lastButtonState == HIGH)
//       {
//         event = true; // button press detected
//       }

//       lastButtonState = reading;
//     }

//     lastReading = reading;

//     return event;
//   }
// }

namespace rotary_encoder
{
  int lastEncoded = 0;
  int encoderValue = 0;
  int lastValue = 0;

  bool lastButtonState = HIGH;
  bool buttonState = HIGH;
  bool lastReading = HIGH;
  bool buttonHeld = false;

  unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;

  unsigned long pressStart = 0;

  void begin()
  {
    pinMode(ENCODER_A, INPUT);
    pinMode(ENCODER_B, INPUT);
    pinMode(ENCODER_SW, INPUT_PULLUP);
  }

  void update()
  {
    int MSB = digitalRead(ENCODER_A);
    int LSB = digitalRead(ENCODER_B);

    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011)
      encoderValue--;

    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000)
      encoderValue++;

    lastEncoded = encoded;

    // ---- Button debounce ----
    bool reading = digitalRead(ENCODER_SW);
    unsigned long now = millis();

    if (reading != lastReading)
      lastDebounceTime = now;

    if ((now - lastDebounceTime) > debounceDelay)
    {
      buttonState = reading;
    }

    lastReading = reading;
  }

  int getDirection()
  {
    int dir = 0;

    if (encoderValue != lastValue)
    {
      if (encoderValue > lastValue)
        dir = 1;
      else
        dir = -1;

      lastValue = encoderValue;
    }

    return dir;
  }

  bool wasPressed()
  {
    bool event = false;

    if (buttonState == LOW && lastButtonState == HIGH)
    {
      event = true;
    }

    lastButtonState = buttonState;

    return event;
  }

  bool isPressed()
  {
    return buttonState == LOW;
  }

  bool isHeld(unsigned long holdTime)
  {
    static bool holdTriggered = false;

    if (digitalRead(ENCODER_SW) == LOW)
    {
      if (!buttonHeld)
      {
        buttonHeld = true;
        pressStart = millis();
        holdTriggered = false;
      }

      if (!holdTriggered && millis() - pressStart >= holdTime)
      {
        holdTriggered = true;
        return true;
      }
    }
    else
    {
      buttonHeld = false;
      holdTriggered = false;
    }

    return false;
  }
}
