#include "WindSensor.h"
#include "Config.h"

WindSensor::Reading WindSensor::read() {
  Reading r;
  float mvSpd = _medianMV(Cfg::PIN_WIND_SPD);
  float mvDir = _medianMV(Cfg::PIN_WIND_DIR);

  // Deadzone 0–140 mV → angin = 0
  if (mvSpd < 140.0f) mvSpd = 0.0f;
  r.speed_ms = mvSpd * (30.0f / 3300.0f);

  r.dir_deg  = (mvDir / 3300.0f) * 360.0f;
  r.dir_deg  = constrain(r.dir_deg, 0.0f, 359.9f);
  return r;
}

float WindSensor::_medianMV(uint8_t pin, uint8_t n) {
  float buf[15];
  for (uint8_t i = 0; i < n; i++) {
    buf[i] = (float)analogReadMilliVolts(pin);
    delayMicroseconds(300);
  }
  // Insertion sort
  for (uint8_t i = 1; i < n; i++) {
    float key = buf[i];
    int   j   = i - 1;
    while (j >= 0 && buf[j] > key) { buf[j + 1] = buf[j]; j--; }
    buf[j + 1] = key;
  }
  return buf[n / 2];
}
