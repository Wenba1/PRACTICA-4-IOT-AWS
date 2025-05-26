#ifndef RGBLED_H
#define RGBLED_H

class RGBLed {
  int redPin, greenPin, bluePin;

public:
  RGBLed(int rPin, int gPin, int bPin);
  void begin();
  void setColor(int r, int g, int b);   // Para control manual por RGB
  void setColor(char color);           // 'r' = rojo, 'g' = verde, 'b' = azul, otro = apagado
  void off();                         // Apagar el LED
};

#endif

