#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>

// ================================================================
//  CLASS: NetworkManager  — WiFi reconnect + OTA + HTTP keep-alive
//  Hanya diakses dari Core 0 (NetTask), tidak perlu mutex internal.
// ================================================================
class NetworkManager {
public:
  void begin();
  bool connected() const { return WiFi.status() == WL_CONNECTED; }

  // Dipanggil periodik dari NetTask: reconnect WiFi + handle OTA
  void tick();

  // Publish JSON payload via MQTT
  bool publishMQTT(const char* payload);

private:
  WiFiClientSecure _secClient;
  PubSubClient     _mqttClient;
  bool             _httpOK   = false;
  uint32_t         _lastPost = 0;
  uint32_t         _lastOTACheck = 0;

  void _reconnectIfNeeded();
  void _reconnectMQTT();
  void _checkHTTPUpdate();
};

#endif // NETWORKMANAGER_H
