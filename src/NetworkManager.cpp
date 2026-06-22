#include "NetworkManager.h"
#include "Config.h"

void NetworkManager::begin() {
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.begin(Cfg::WIFI_SSID, Cfg::WIFI_PASS);
  // Tunggu koneksi awal (maks 10 detik)
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) delay(500);

  _secClient.setInsecure();  // Skip TLS cert verification
  
  _mqttClient.setClient(_secClient);
  _mqttClient.setServer(Cfg::MQTT_BROKER, Cfg::MQTT_PORT);
}

void NetworkManager::tick() {
  _reconnectIfNeeded();
  _reconnectMQTT();
  _mqttClient.loop();
  _checkHTTPUpdate();
}

bool NetworkManager::publishMQTT(const char* payload) {
  if (!connected() || !_mqttClient.connected()) return false;
  return _mqttClient.publish(Cfg::MQTT_TOPIC, payload);
}

void NetworkManager::_reconnectMQTT() {
  if (!connected()) return;
  if (_mqttClient.connected()) return;

  static uint32_t lastTry = 0;
  if (millis() - lastTry < 5000) return; // Coba reconnect setiap 5 detik
  lastTry = millis();

  String clientId = "TPA-Monitor-";
  clientId += String(random(0xffff), HEX);

  // Jika broker butuh username & password
  disableCore0WDT();
  if (strlen(Cfg::MQTT_USER) > 0) {
    if (_mqttClient.connect(clientId.c_str(), Cfg::MQTT_USER, Cfg::MQTT_PASS)) {
      Serial.println("[MQTT] Terhubung!");
    } else {
      Serial.print("[MQTT] Gagal konek, rc=");
      Serial.println(_mqttClient.state());
    }
  } else {
    // Tanpa username & password
    if (_mqttClient.connect(clientId.c_str())) {
      Serial.println("[MQTT] Terhubung (tanpa kredensial)!");
    } else {
      Serial.print("[MQTT] Gagal konek, rc=");
      Serial.println(_mqttClient.state());
    }
  }
  enableCore0WDT();
}

void NetworkManager::_reconnectIfNeeded() {
  static uint32_t lastTry  = 0;
  static uint8_t  failCnt  = 0;

  if (connected()) { failCnt = 0; return; }
  if (millis() - lastTry < 3000) return;
  lastTry = millis();
  failCnt++;

  _httpOK = false;
  WiFi.disconnect();
  delay(200);
  WiFi.begin(Cfg::WIFI_SSID, Cfg::WIFI_PASS);
  for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) delay(500);

  // Hard reset WiFi setelah 5× gagal berturut-turut
  if (!connected() && failCnt >= 5) {
    WiFi.mode(WIFI_OFF);
    delay(1000);
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    WiFi.begin(Cfg::WIFI_SSID, Cfg::WIFI_PASS);
    failCnt = 0;
  }
}

void NetworkManager::_checkHTTPUpdate() {
  if (!connected()) return;

  // Hanya jalankan cek OTA jika interval sudah terlampaui atau ini baru nyala
  if (_lastOTACheck != 0 && millis() - _lastOTACheck < Cfg::OTA_CHECK_MS) {
    return;
  }
  _lastOTACheck = millis();
  if (_lastOTACheck == 0) _lastOTACheck = 1; // hindari angka 0

  Serial.println("[OTA] Cek versi baru di server...");

  disableCore0WDT();
  HTTPClient http;
  if (http.begin(_secClient, Cfg::OTA_VERSION_URL)) {
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String serverVersion = http.getString();
      serverVersion.trim(); // Hilangkan spasi / newline
      
      Serial.printf("[OTA] Versi Server: '%s', Versi Alat: '%s'\n", serverVersion.c_str(), Cfg::FIRMWARE_VERSION);

      if (serverVersion.length() > 0 && serverVersion != String(Cfg::FIRMWARE_VERSION)) {
        Serial.println("[OTA] Versi baru terdeteksi! Memulai download firmware...");
        
        httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
        t_httpUpdate_return ret = httpUpdate.update(_secClient, Cfg::OTA_BIN_URL);

        switch (ret) {
          case HTTP_UPDATE_FAILED:
            Serial.printf("[OTA] Update gagal (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
            break;
          case HTTP_UPDATE_NO_UPDATES:
            Serial.println("[OTA] Tidak ada update baru.");
            break;
          case HTTP_UPDATE_OK:
            Serial.println("[OTA] Update sukses! Rebooting...");
            break;
        }
      } else {
        Serial.println("[OTA] Firmware sudah versi terbaru.");
      }
    } else {
      Serial.printf("[OTA] Gagal cek versi HTTP code: %d\n", httpCode);
    }
    http.end();
  }
  enableCore0WDT();
}
