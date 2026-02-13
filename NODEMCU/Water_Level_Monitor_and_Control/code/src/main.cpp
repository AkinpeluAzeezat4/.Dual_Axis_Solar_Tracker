#include <Arduino.h>
#include <Wire.h>

#define TRIG_PIN D6
#define ECHO_PIN D5
#define RELAY_PIN D7
#define PCA9555_ADDR 0x20
#define TANK_DEPTH_CM 9

// LED bar mapping (0% → 100%)
uint16_t levelBits[11] = {
    0b0000000100000000, // 0%
    0b0000001100000000, // 20%
    0b0000011100000000, // 10%
    0b0000011110000000, // 30%
    0b0000011111000000, // 40%
    0b0000011111100000, // 50%
    0b0000011111110000, // 60%
    0b0000011111111000, // 70%
    0b0000011111111100, // 80%
    0b0000011111111110, // 90%
    0b0000011111111111  // 100%
};

// For smoothing
#define SMOOTH_COUNT 5
long distanceBuffer[SMOOTH_COUNT] = {0};
int bufferIndex = 0;

long readDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0)
    return -1;

  long distance = duration * 0.0343 / 2;

  // store in buffer for averaging
  distanceBuffer[bufferIndex] = distance;
  bufferIndex = (bufferIndex + 1) % SMOOTH_COUNT;

  long sum = 0;
  int count = 0;
  for (int i = 0; i < SMOOTH_COUNT; i++)
  {
    if (distanceBuffer[i] > 0)
    {
      sum += distanceBuffer[i];
      count++;
    }
  }
  if (count == 0)
    return -1;
  return sum / count;
}

void pcaWrite(uint16_t value)
{
  Wire.beginTransmission(PCA9555_ADDR);
  Wire.write(0x02); // Output register 0
  Wire.write(value & 0xFF);
  Wire.write(value >> 8);
  Wire.endTransmission();
}

void setup()
{
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  Wire.begin(D2, D1);
  Wire.setClock(100000);

  // configure PCA9555 pins as output
  Wire.beginTransmission(PCA9555_ADDR);
  Wire.write(0x06);
  Wire.write(0x00); // Port 0 output
  Wire.write(0x00); // Port 1 output
  Wire.endTransmission();
  pcaWrite(0x0000); // all LEDs off
  digitalWrite(RELAY_PIN, LOW);
}

void loop()
{
  long distance = readDistance();

  int percentage = ((TANK_DEPTH_CM - distance) * 100) / TANK_DEPTH_CM;
  percentage = constrain(percentage, 0, 100);

  int barIndex = percentage / 7;
  if (barIndex > 9)
    barIndex = 10;
  pcaWrite(levelBits[barIndex]);
  if (barIndex > 9)
    digitalWrite(RELAY_PIN, LOW);
  else if (barIndex <= 1)
  {
    digitalWrite(RELAY_PIN, HIGH);
  }
  delay(100);
}
