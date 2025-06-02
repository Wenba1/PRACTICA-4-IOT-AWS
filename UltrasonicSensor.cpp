#include "UltrasonicSensor.h"
#include <Arduino.h>

UltrasonicSensor::UltrasonicSensor(int triggerPin, int echoPin, float& lastDistance)
    : trigPin(triggerPin), echoPin(echoPin), lastDistance(lastDistance) {}

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
  if (newDistance <= 0 || newDistance >= 15) {
    return false;
  }

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
