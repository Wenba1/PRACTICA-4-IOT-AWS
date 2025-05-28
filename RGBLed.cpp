#include "RGBLed.h"

RGBLed::RGBLed(int rPin, int gPin, int bPin)
  : redPin(rPin), greenPin(gPin), bluePin(bPin) {}

void RGBLed::begin() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void RGBLed::setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void RGBLed::setColor(const char* color) {
   if (strcmp(color, "red") == 0) setColor(255, 0, 0);
  else if (strcmp(color, "green") == 0) setColor(0, 255, 0);
  else if (strcmp(color, "blue") == 0) setColor(0, 0, 255);
}