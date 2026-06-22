#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include <Arduino.h>

struct SensorData {
  float    wind_speed  = 0.0f;
  float    wind_dir    = 0.0f;
  uint16_t pm1         = 0;
  uint16_t pm25        = 0;
  uint16_t pm10        = 0;
  uint16_t co2         = 0;
  float    temperature = 0.0f;
  float    humidity    = 0.0f;
  bool     fan         = false;
  uint32_t ts_ms       = 0;
  bool     fresh       = false;   // flag: data baru belum dikirim
};

extern SensorData        g_data;
extern SemaphoreHandle_t g_mutex;

#endif // SHAREDDATA_H
