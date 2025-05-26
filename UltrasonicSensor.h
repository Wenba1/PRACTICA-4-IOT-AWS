#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>

class UltrasonicSensor {
  int trigPin, echoPin;

public:
  UltrasonicSensor(int trig, int echo);
  void begin();
  float getDistance();
  int getLevel(float distance);
  String getFillingState(int level); // ← importante
};

#endif

