#ifndef WINDSENSOR_H
#define WINDSENSOR_H

#include <Arduino.h>

// ================================================================
//  CLASS: WindSensor  — ADC median filter (speed + direction)
// ================================================================
class WindSensor {
public:
  struct Reading {
    float speed_ms = 0.0f;
    float dir_deg  = 0.0f;
  };

  Reading read();

private:
  static float _medianMV(uint8_t pin, uint8_t n = 15);
};

#endif // WINDSENSOR_H
