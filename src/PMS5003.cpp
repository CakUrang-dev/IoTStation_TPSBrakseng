#include "PMS5003.h"
#include "Config.h"

void PMS5003::begin() {
  Serial2.begin(9600, SERIAL_8N1, Cfg::PMS_RX, Cfg::PMS_TX);
  _idx = 0;
}

bool PMS5003::update() {
  while (Serial2.available()) {
    uint8_t b = (uint8_t)Serial2.read();

    if (_idx == 0) {
      if (b == 0x42) _buf[_idx++] = b;
      // else: buang, tetap di state 0

    } else if (_idx == 1) {
      if (b == 0x4D) {
        _buf[_idx++] = b;
      } else if (b == 0x42) {
        // Mungkin awal frame baru
        _buf[0] = b;
        _idx    = 1;
      } else {
        _idx = 0;  // reset
      }

    } else {
      _buf[_idx++] = b;
      if (_idx == 32) {
        _idx = 0;
        return _parse();
      }
    }
  }
  return false;
}

bool PMS5003::_parse() {
  uint16_t sum = 0;
  for (int i = 0; i < 30; i++) sum += _buf[i];
  uint16_t chkRecv = ((uint16_t)_buf[30] << 8) | _buf[31];
  
  if (sum != chkRecv) {
    return false;
  }

  _frame.pm1   = ((uint16_t)_buf[10]  << 8) | _buf[11];
  _frame.pm25  = ((uint16_t)_buf[12]  << 8) | _buf[13];
  _frame.pm10  = ((uint16_t)_buf[14]  << 8) | _buf[15];
  _frame.valid = true;
  return true;
}
