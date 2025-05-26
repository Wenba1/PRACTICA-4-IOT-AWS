#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>
#include "ServoController.h"
#include "RGBLed.h"
#include "UltrasonicSensor.h"

class MQTTManager {
  public:
    MQTTManager(PubSubClient& client, ServoController* servo, RGBLed* led, UltrasonicSensor* ultrasonic, const char* topic,
                bool& motion, unsigned long& motionTime, float& distance, int& level,
                bool& door, unsigned char& ledColor, bool& autoMode);

    void setCallback();
    void reconnectMQTT(const char* clientId);
    void publishState();

  private:
    PubSubClient& mqttClient;
    ServoController* servo;
    RGBLed* rgbLed;
    UltrasonicSensor* ultrasonic;
    const char* updateTopic;

    bool& motionDetected;
    unsigned long& lastMotionTime;
    float& lastDistance;
    int& lastLevel;
    bool& doorOpen;
    unsigned char& color_led;
    bool& automatic_mode;

    void handleShadowUpdate(const char* payload);
};

#endif
