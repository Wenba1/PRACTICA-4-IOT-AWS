#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WiFiClientSecure.h>

class WiFiManager {
private:
  const char* ssid;
  const char* password;

public:
  WiFiManager(const char* ssid, const char* password);
  void connectWiFi();
  void configureCertificates(WiFiClientSecure& wiFiClient, const char* rootCA, const char* certificate, const char* privateKey);
};

#endif

