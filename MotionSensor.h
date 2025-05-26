#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Arduino.h>

class MotionSensor {
private:
    int pin;
    bool lastState;

public:
    MotionSensor(int pin);  // Constructor
    void begin();           // Inicializa el pin
    bool detectMotionForDuration(unsigned long durationMs);    // Detecta flanco ascendente
    bool detectMotionOverTime(unsigned long durationMs = 2000); // Lee durante 2s
};

#endif
