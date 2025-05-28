#ifndef SERVO_CONTROLLER_H
#define SERVO_CONTROLLER_H

#include <ESP32Servo.h>

class ServoController {
private:
  Servo servo;
  int servoPin;

public:
  ServoController(int pin);
  void begin();
  void open();
  void close();
};

#endif