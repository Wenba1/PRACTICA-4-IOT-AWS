#include "UltrasonicSensor.h"
#include <Arduino.h>

UltrasonicSensor::UltrasonicSensor(int triggerPin, int echoPin, float lastDistance)
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
  long duration = pulseIn(echoPin, HIGH, 30000); 
  float distance = duration * 0.0343 / 2;
  return (duration == 0) ? 100.0 : distance;
}

bool UltrasonicSensor::depthSignificantChange(float newDistance, float threshold) {
  if (lastDistance < 0) {
    lastDistance = newDistance; 
    return true;
  }

  if (abs(newDistance - lastDistance) >= threshold) {
    lastDistance = newDistance;
    return true;
  }

  return false;
}

int UltrasonicSensor::getLevel(float distance) {
  if (distance <= 5) return 2;           
  else if (distance <= 15) return 1;     
  else return 0;                         
}

String UltrasonicSensor::getFillingState(int level) {
  if (level == 2) return "full";
  else if (level == 1) return "half_full";
  else return "empty";
}
