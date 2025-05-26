#include "ServoController.h"

ServoController::ServoController(int pin) : servoPin(pin) {}

void ServoController::begin() {
  servo.attach(servoPin);
  close();
}

void ServoController::open() {
  servo.write(90);
}

void ServoController::close() {
  servo.write(0);
}
