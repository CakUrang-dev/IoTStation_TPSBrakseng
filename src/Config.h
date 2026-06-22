#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "secrets.h"

namespace Cfg {
  constexpr char     WIFI_SSID[]    = SECRET_WIFI_SSID;
  constexpr char     WIFI_PASS[]    = SECRET_WIFI_PASS;
  
  constexpr char     MQTT_BROKER[]  = "b846d696536846c98fd91529e90260aa.s1.eu.hivemq.cloud"; 
  constexpr uint16_t MQTT_PORT      = 8883;
  constexpr char     MQTT_USER[]    = SECRET_MQTT_USER;
  constexpr char     MQTT_PASS[]    = SECRET_MQTT_PASS;
  constexpr char     MQTT_TOPIC[]   = "tpa/monitor/data";
  constexpr char     FIRMWARE_VERSION[] = "1.0.0"; // Versi saat ini
  constexpr char     OTA_VERSION_URL[]  = "https://raw.githubusercontent.com/USER/REPO/main/version.txt";
  constexpr char     OTA_BIN_URL[]      = "https://raw.githubusercontent.com/USER/REPO/main/firmware.bin";
  constexpr uint32_t OTA_CHECK_MS       = 3600000; // Cek update setiap 1 jam

  constexpr uint8_t  SDA_PIN        = 21;
  constexpr uint8_t  SCL_PIN        = 22;
  constexpr uint8_t  PIN_WIND_SPD   = 32;
  constexpr uint8_t  PIN_WIND_DIR   = 35;
  constexpr uint8_t  PIN_FAN_POWER  = 26;
  constexpr uint8_t  PIN_FAN_DIR    = 33;
  constexpr uint8_t  PMS_RX         = 17;   // Serial2 RX ← PMS5003 TX
  constexpr uint8_t  PMS_TX         = 16;   // Serial2 TX → PMS5003 RX

  constexpr uint32_t SENSOR_MS      = 2000;
  constexpr uint32_t WATCHDOG_MS    = 8000;
  constexpr uint32_t FAN_INTAKE_MS  = 30000UL;   // 30 detik menyedot udara
  constexpr uint32_t FAN_DELAY_MS   = 30000UL;   // 30 detik diam untuk baca PMS
  constexpr uint32_t FAN_EXHAUST_MS = 30000UL;   // 30 detik membuang udara
  constexpr uint32_t FAN_IDLE_MS    = 120000UL;  // 2 menit istirahat
}

#endif // CONFIG_H
