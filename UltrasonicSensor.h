#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor {
  int trigPin, echoPin;
  float lastDistance;

public:
  UltrasonicSensor(int trig, int echo, float lastDistance);
  void begin();
  float getDistance();
  int getLevel(float distance);
  String getFillingState(int level);
  bool depthSignificantChange(float newDistance, float threshold = 15);
};

#endif
