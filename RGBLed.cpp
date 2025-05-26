#include "RGBLed.h"
#include <Arduino.h>

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

void RGBLed::setColor(char color) {
  if (color == 'r') setColor(255, 0, 0);       // rojo
  else if (color == 'g') setColor(0, 255, 0);  // verde
  else if (color == 'b') setColor(0, 0, 255);  // azul
  else off();                                  // cualquier otro -> apagado
}

void RGBLed::off() {
  setColor(0, 0, 0);  // apaga todos los canales
}

