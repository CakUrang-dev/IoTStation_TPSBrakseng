#include "SensorNode.h"

void SensorNode::begin() {
  Serial.begin(115200);
  Wire.begin(Cfg::SDA_PIN, Cfg::SCL_PIN);
  Wire.setClock(400000);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // Init display → tampilkan boot message
  _display.begin();
  _display.printLine("TPA Monitor v2.0");
  _display.printLine("Connecting WiFi...");
  _display.printLine(Cfg::WIFI_SSID);

  // Init network (blok hingga koneksi awal atau timeout)
  _net.begin();
  _display.printLine(_net.connected() ? "WiFi OK!" : "Offline mode");

  // Init hardware
  _fan.begin();
  _pms.begin();
  _sht31Ok = _sht31.begin(0x44);
  _scd4x.begin(Wire, SCD41_I2C_ADDR_62);
  _scd4x.startPeriodicMeasurement();

  // Buat mutex shared data
  g_mutex = xSemaphoreCreateMutex();
  configASSERT(g_mutex);

  // Spawn Network Task di Core 0, priority 2
  BaseType_t ok = xTaskCreatePinnedToCore(
    _netTask, "NetTask",
    10240,        // stack bytes
    this,         // parameter (pointer ke SensorNode)
    2,            // priority
    nullptr,      // handle tidak diperlukan
    0             // Core 0
  );
  configASSERT(ok == pdPASS);

  delay(2000);
  _display.begin();  // clear OLED setelah boot screen
}

void SensorNode::loop() {
  static uint32_t lastSensor = 0;
  static uint16_t co2        = 0;

  // Fan controller: harus dipanggil setiap loop (timer-based)
  _fan.update();

  // PMS5003 non-blocking: poll setiap iterasi loop
  if (_pms.update()) {
    _pmsFrame = _pms.get();
  }

  // Baca sensor lain setiap SENSOR_MS
  if (millis() - lastSensor < Cfg::SENSOR_MS) return;
  lastSensor = millis();

  static float lastT = 0.0f;
  static float lastH = 0.0f;

  // --- SCD4x: CO2, Temp, Hum (I2C) ---
  bool ready = false;
  uint16_t scdError = _scd4x.getDataReadyStatus(ready);
  
  if (scdError == 0 && ready) {
    float t, h;
    if (_scd4x.readMeasurement(co2, t, h) == 0) {
      lastT = t;
      lastH = h;
    }
  } else if (scdError != 0) {
    // Fallback ke SHT31 jika SCD41 bermasalah/terputus
    if (_sht31Ok) {
      lastT = _sht31.readTemperature();
      lastH = _sht31.readHumidity();
    }
  }
  delay(50);

  // --- Wind: speed + direction (ADC, ~15 samples × 2 channel) ---
  WindSensor::Reading wind = _wind.read();
  delay(50);

  // --- Update shared data (protected by mutex) ---
  if (xSemaphoreTake(g_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
    g_data.wind_speed  = wind.speed_ms;
    g_data.wind_dir    = wind.dir_deg;
    g_data.pm1         = _pmsFrame.valid ? _pmsFrame.pm1  : g_data.pm1;
    g_data.pm25        = _pmsFrame.valid ? _pmsFrame.pm25 : g_data.pm25;
    g_data.pm10        = _pmsFrame.valid ? _pmsFrame.pm10 : g_data.pm10;
    g_data.co2         = co2;
    g_data.temperature = lastT;
    g_data.humidity    = lastH;
    g_data.fan         = _fan.isOn();
    g_data.ts_ms       = millis();
    g_data.fresh       = true;
    xSemaphoreGive(g_mutex);
  }

  // --- OLED update (I2C, hanya dari Core 1 → aman tanpa mutex) ---
  _display.render(g_data);
}

void SensorNode::_netTask(void* arg) {
  SensorNode* self = static_cast<SensorNode*>(arg);
  char payload[280];

  while (true) {
    // WiFi reconnect + OTA handle
    self->_net.tick();

    // Ambil snapshot data jika fresh
    bool       send = false;
    SensorData snap;
    if (xSemaphoreTake(g_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
      if (g_data.fresh) {
        snap        = g_data;
        g_data.fresh = false;
        send         = true;
      }
      xSemaphoreGive(g_mutex);
    }

    if (send) {
      snprintf(payload, sizeof(payload),
        "{"
        "\"wind_speed\":%.2f,"
        "\"wind_dir\":%.1f,"
        "\"pm1\":%u,"
        "\"pm25\":%u,"
        "\"pm10\":%u,"
        "\"co2\":%u,"
        "\"temperature\":%.2f,"
        "\"humidity\":%.2f,"
        "\"fan\":%s,"
        "\"measured_at\":%lu"
        "}",
        snap.wind_speed,
        snap.wind_dir,
        snap.pm1,
        snap.pm25,
        snap.pm10,
        snap.co2,
        snap.temperature,
        snap.humidity,
        snap.fan ? "true" : "false",
        (unsigned long)snap.ts_ms
      );
      Serial.print("[DATA] ");
      Serial.println(payload);
      // self->_net.publishMQTT(payload);
    }

    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
