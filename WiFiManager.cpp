#include "WiFiManager.h"
#include <Arduino.h>

WiFiManager::WiFiManager(const char* ssid, const char* password) : ssid(ssid), password(password) {}

void WiFiManager::connectWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void WiFiManager::configureCertificates(WiFiClientSecure& wiFiClient, const char* rootCA, const char* certificate, const char* privateKey) {
  wiFiClient.setCACert(rootCA);
  wiFiClient.setCertificate(certificate);
  wiFiClient.setPrivateKey(privateKey);
  Serial.println("Certificates configured for AWS IoT.");
}

