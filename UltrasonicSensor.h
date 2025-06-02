#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor {
  int trigPin, echoPin;
  float& lastDistance;

public:
  UltrasonicSensor(int triggerPin, int echoPin, float& lastDistance);
  void begin();
  float getDistance();
  bool depthSignificantChange(float newDistance, float threshold = 15);
};

#endif


