#include "UltrasonicSensor.h"
#include <Arduino.h>

UltrasonicSensor::UltrasonicSensor(int triggerPin, int echoPin)
    : trigPin(triggerPin), echoPin(echoPin) {}

void UltrasonicSensor::begin() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

float UltrasonicSensor::getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);  // timeout 30 ms
  float distance = duration * 0.0343 / 2;
  return (duration == 0) ? 100.0 : distance;
}

int UltrasonicSensor::getLevel(float distance) {
  if (distance <= 3) return 2;           // lleno
  else if (distance <= 10) return 1;     // medio
  else return 0;                         // vacío
}

String UltrasonicSensor::getFillingState(int level) {
  if (level == 2) return "full";
  else if (level == 1) return "medium";
  else return "empty";
}
