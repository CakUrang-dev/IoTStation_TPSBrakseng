#ifndef PMS5003_H
#define PMS5003_H

#include <Arduino.h>

// ================================================================
//  CLASS: PMS5003  — non-blocking UART frame parser
//
//  Frame 32 byte:
//    [0]  0x42  [1] 0x4D  [2-3] len=28
//    [4-5] PM1  [6-7] PM2.5  [8-9] PM10  (CF=1, standar)
//    …   [30-31] checksum = sum([0..29])
// ================================================================
class PMS5003 {
public:
  struct Frame {
    uint16_t pm1   = 0;
    uint16_t pm25  = 0;
    uint16_t pm10  = 0;
    bool     valid = false;
  };

  void begin();

  // Panggil setiap loop iteration (non-blocking).
  // Kembalikan true saat frame baru berhasil di-parse.
  bool update();

  Frame get() const { return _frame; }

private:
  uint8_t _buf[32] = {};
  uint8_t _idx     = 0;
  Frame   _frame;

  bool _parse();
};

#endif // PMS5003_H
