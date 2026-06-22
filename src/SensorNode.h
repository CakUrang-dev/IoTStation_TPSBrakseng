#ifndef SENSORNODE_H
#define SENSORNODE_H

#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cScd4x.h>
#include <Adafruit_SHT31.h>

#include "Config.h"
#include "SharedData.h"
#include "PMS5003.h"
#include "WindSensor.h"
#include "FanController.h"
#include "Display.h"
#include "NetworkManager.h"

// ================================================================
//  CLASS: SensorNode  — orkestrator utama
//
//  Pola akses:
//    Core 1 (loop)    → PMS5003, WindSensor, SCD4x, SHT31, Display, FanController
//    Core 0 (NetTask) → NetworkManager
//    Shared (mutex)   → g_data
// ================================================================
class SensorNode {
public:
  void begin();
  void loop();

private:
  PMS5003           _pms;
  PMS5003::Frame    _pmsFrame;
  WindSensor        _wind;
  FanController     _fan;
  Display           _display;
  NetworkManager    _net;
  SensirionI2cScd4x _scd4x;
  Adafruit_SHT31    _sht31;
  bool              _sht31Ok = false;

  // ── FreeRTOS Task: Network (Core 0) ──────────────────────────
  static void _netTask(void* arg);
};

#endif // SENSORNODE_H
