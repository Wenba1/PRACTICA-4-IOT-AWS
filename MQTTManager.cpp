#include "MQTTManager.h"
#include <ArduinoJson.h>

MQTTManager::MQTTManager(PubSubClient& client, ServoController* servo, RGBLed* led, UltrasonicSensor* ultrasonic, const char* topic,
                         bool& motion, unsigned long& motionTime, float& distance, int& level,
                         bool& door, String& ledColor, bool& autoMode, String& fillingState, bool* openFlag, bool* closeFlag)
  : mqttClient(client), servo(servo), rgbLed(led), ultrasonic(ultrasonic), updateTopic(topic),
    motionDetected(motion), lastMotionTime(motionTime), lastDistance(distance), lastLevel(level),
    doorOpen(door), color_led(ledColor), automatic_mode(autoMode), filling_state(fillingState), shouldOpenLid(openFlag), shouldCloseLid(closeFlag) {}

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

  doc["state"]["reported"]["lid_open"] = doorOpen;
  doc["state"]["reported"]["depth_cm"] = lastDistance;
  doc["state"]["reported"]["filling_state"] = filling_state.c_str();
  doc["state"]["reported"]["led_color"] = color_led.c_str();

  char buffer[512];
  serializeJson(doc, buffer);
  Serial.println("Publishing status:");
  Serial.println(buffer);
  mqttClient.publish(updateTopic, buffer);
}

void MQTTManager::handleShadowUpdate(const char* payload) {
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("Error JSON: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject desired = doc["state"];

  bool stateChanged = false;

  if (desired.containsKey("lid_open")) {

    bool open = desired["lid_open"];
    Serial.print("Actual value doorOpen: ");
    Serial.println(doorOpen);
    if (open != doorOpen) {
      doorOpen = open;
      Serial.println("lid state changed. action executed.");
      if (open) {
        Serial.println("Request to open lid.");
        *shouldOpenLid = true;
      } else {
        Serial.println("Request to close lid.");
        *shouldCloseLid = true;
      }
      stateChanged = true;
    }
  }

  if (desired.containsKey("led_color")) {
    const char* color = desired["led_color"];
    if (color_led != String(color)) {
      color_led = color;
      Serial.println(color);
      rgbLed->setColor(color);
      stateChanged = true;
    }
  }

  if (desired.containsKey("filling_state")) {
    const char* newFillingState = desired["filling_state"];
    if (filling_state != String(newFillingState)) {
      filling_state = String(newFillingState);
      Serial.println(filling_state);
      stateChanged = true;
    }
  }

  if (stateChanged) {
    publishState();
  }
}