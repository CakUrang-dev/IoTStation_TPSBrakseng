#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>

// State Machine Kipas Ganda
enum FanState {
  STATE_INTAKE,       // Nyedot udara
  STATE_DELAY_READ,   // Diam, stabilisasi PMS
  STATE_EXHAUST,      // Buang udara
  STATE_IDLE          // Istirahat panjang
};

class FanController {
public:
  void begin();
  void update();
  
  // Mengembalikan true jika power kipas sedang nyala
  bool isOn() const { 
    return _state == STATE_INTAKE || _state == STATE_EXHAUST; 
  }

private:
  FanState _state = STATE_IDLE;
  uint32_t _timer = 0;
};

#endif // FAN_CONTROLLER_H
