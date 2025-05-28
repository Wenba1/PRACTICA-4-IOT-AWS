#ifndef RGBLED_H
#define RGBLED_H

#include <Arduino.h>

class RGBLed {
  int redPin, greenPin, bluePin;

public:
  RGBLed(int rPin, int gPin, int bPin);
  void begin();
  void setColor(int r, int g, int b);
  void setColor(const char* color); 

};

#endif

