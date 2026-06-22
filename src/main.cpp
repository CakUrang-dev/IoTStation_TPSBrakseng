// ================================================================
//  TPA Monitor – ESP32 Environmental Station  v2.0
//  Refactored: OOP | FreeRTOS dual-core | ArduinoOTA | PMS5003
//
//  Core 0 (NetTask) : WiFi reconnect + ArduinoOTA + HTTP POST
//  Core 1 (loop)    : PMS5003 poll + Sensor read + OLED update
// ================================================================

#include "SensorNode.h"

// ================================================================
//  ENTRY POINT
// ================================================================
static SensorNode node;

void setup() { 
  node.begin(); 
}

void loop() { 
  node.loop();  
}
