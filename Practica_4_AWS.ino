#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "MotionSensor.h"
#include "ServoController.h"
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "RGBLed.h"
#include "UltrasonicSensor.h"

// Pines
const int PIR_PIN = 19;
const int SERVO_PIN = 18;
const int RGB_R_PIN = 21;
const int RGB_G_PIN = 23;
const int RGB_B_PIN = 22;
const int TRIGGER_PIN = 2;
const int ECHO_PIN = 4;

// Objetos hardware
WiFiClientSecure espClient;
PubSubClient mqttClient(espClient);

MotionSensor motionSensor(PIR_PIN);
ServoController servo(SERVO_PIN);
RGBLed rgbLed(RGB_R_PIN, RGB_G_PIN, RGB_B_PIN);
UltrasonicSensor ultrasonic(TRIGGER_PIN, ECHO_PIN);

// Config WiFi y MQTT
const char* WIFI_SSID = "Familia Salinas";
const char* WIFI_PASSWORD = "5278901cbba";
const char* MQTT_BROKER = "ahxlba0g2uxk8-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "id_trash_can";
const char* UPDATE_TOPIC = "$aws/things/trash_can/shadow/update";

// Certificados (rellena con tus datos)
const char AMAZON_PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
... Tu clave privada aquí ...
-----END RSA PRIVATE KEY-----
)KEY";

const char AMAZON_CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
... Tu certificado aquí ...
-----END CERTIFICATE-----
)KEY";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
... Amazon Root CA1 aquí ...
-----END CERTIFICATE-----
)EOF";

// Estado del sistema
bool motionDetected = false;
unsigned long lastMotionTime = 0;
float lastDistance = 0;
int lastLevel = 0;
bool doorOpen = false;
unsigned char color_led = 'g';
bool automatic_mode = true; // Solo se reporta, no se utiliza en lógica

// WiFi helper
WiFiManager wifi(WIFI_SSID, WIFI_PASSWORD);

// MQTT manager
MQTTManager mqtt(mqttClient, &servo, &rgbLed, &ultrasonic, UPDATE_TOPIC,
                 motionDetected, lastMotionTime, lastDistance, lastLevel, doorOpen, color_led, automatic_mode);

void setup() {
  Serial.begin(115200);
  delay(1000);

  wifi.connectWiFi();
  wifi.configureCertificates(espClient, AMAZON_ROOT_CA1, AMAZON_CERTIFICATE, AMAZON_PRIVATE_KEY);

  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback();

  motionSensor.begin();
  servo.begin();
  servo.close();

  rgbLed.begin();
  ultrasonic.begin();

  Serial.println("Sistema listo. Esperando comandos...");
}

void loop() {
  if (!mqttClient.connected()) {
    mqtt.reconnectMQTT(CLIENT_ID);
  }
  mqttClient.loop();

  bool currentMotion = motionSensor.detectMotionOverTime();
  if (currentMotion && !motionDetected) {
    motionDetected = true;
    lastMotionTime = millis();
    Serial.println("¡Movimiento detectado!");
    mqtt.publishState();
  }
  if (!currentMotion && motionDetected) {
    motionDetected = false;
    mqtt.publishState();
  }

  float distance = ultrasonic.getDistance();
  int level = ultrasonic.getLevel(distance);

  if (level != lastLevel) {
    lastLevel = level;
    lastDistance = distance;
    mqtt.publishState();
  }

  delay(100);
}
