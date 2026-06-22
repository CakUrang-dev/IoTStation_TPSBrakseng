#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "SharedData.h"

// ================================================================
//  CLASS: Display  — U8G2 SH1107 128×128 via I2C
//  Hanya diakses dari Core 1 (loop), tidak perlu mutex.
// ================================================================
class Display {
public:
  void begin();
  void printLine(const char* msg);
  void render(const SensorData& d);

private:
  U8G2_SH1107_SEEED_128X128_F_HW_I2C _oled{U8G2_R2, U8X8_PIN_NONE};
};

#endif // DISPLAY_H
