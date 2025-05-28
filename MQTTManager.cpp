#include "MQTTManager.h"
#include <ArduinoJson.h>

MQTTManager::MQTTManager(PubSubClient& client, ServoController* servo, RGBLed* led, UltrasonicSensor* ultrasonic, const char* topic,
                         bool& motion, unsigned long& motionTime, float& distance, int& level,
                         bool& door, const char* ledColor, bool& autoMode)
  : mqttClient(client), servo(servo), rgbLed(led), ultrasonic(ultrasonic), updateTopic(topic),
    motionDetected(motion), lastMotionTime(motionTime), lastDistance(distance), lastLevel(level),
    doorOpen(door), color_led(ledColor), automatic_mode(autoMode) {}

void MQTTManager::setCallback() {
  mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    handleShadowUpdate((const char*)payload);
  });
}


void MQTTManager::reconnectMQTT(const char* clientId) {
  while (!mqttClient.connected()) {
    if (mqttClient.connect(clientId)) {
      mqttClient.subscribe("$aws/things/smart_trash_can_sn0001/shadow/update/delta");
    } else {
      delay(2000);
    }
  }
}

void MQTTManager::publishState() {
  StaticJsonDocument<512> doc;

  doc["state"]["reported"]["motion_detected"] = motionDetected;
  doc["state"]["reported"]["lid_open"] = doorOpen;
  doc["state"]["reported"]["depth_cm"] = lastDistance;
  doc["state"]["reported"]["filling_state"] = ultrasonic->getFillingState(lastLevel);
  doc["state"]["reported"]["led_color"] = color_led;
  doc["state"]["reported"]["automatic_mode"] = automatic_mode;

  char buffer[512];
  serializeJson(doc, buffer);
  mqttClient.publish(updateTopic, buffer);
}

void MQTTManager::handleShadowUpdate(const char* payload) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) return;

  JsonObject desired = doc["state"];

  if (desired.containsKey("lid_open")) {
    bool open = desired["lid_open"];
    if (open) {
      servo->open();
      doorOpen = true;
    } else {
      servo->close();
      doorOpen = false;
    }
  }

  if (desired.containsKey("led_color")) {
  const char* color = desired["led_color"];
  color_led = color; 
  rgbLed->setColor(color);
}
  if (desired.containsKey("automatic_mode")) {
    automatic_mode = desired["automatic_mode"];
  }

  publishState();
}
