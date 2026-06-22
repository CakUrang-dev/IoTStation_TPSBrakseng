#include "FanController.h"
#include "Config.h"

// Logika Relay Modul Biru (Active-LOW) untuk arah kipas (Pin 33)
// LOW  = Relay menyala (terhubung ke NO / Kipas Buang)
// HIGH = Relay mati (terhubung ke NC / Kipas Masuk)
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// Logika N-Channel MOSFET (Active-HIGH) untuk sakelar Ground (Pin 26)
// HIGH = Gate terisi, Mosfet menyambung (Kipas mendapat Ground, berputar)
// LOW  = Gate kosong, Mosfet memutus (Kipas kehilangan Ground, mati total)
#define MOSFET_ON  HIGH
#define MOSFET_OFF LOW

void FanController::begin() {
  pinMode(Cfg::PIN_FAN_POWER, OUTPUT);
  pinMode(Cfg::PIN_FAN_DIR, OUTPUT);
  
  // Matikan semua saat pertama kali menyala
  digitalWrite(Cfg::PIN_FAN_POWER, MOSFET_OFF);
  digitalWrite(Cfg::PIN_FAN_DIR, RELAY_OFF);
  
  _timer = millis();
  _state = STATE_IDLE;
}

void FanController::update() {
  uint32_t now = millis();
  
  switch (_state) {
    case STATE_IDLE:
      if (now - _timer >= Cfg::FAN_IDLE_MS) {
        _state = STATE_INTAKE;
        _timer = now;
        // NC = Fan Masuk (Intake), Relay tidak boleh diaktifkan
        digitalWrite(Cfg::PIN_FAN_DIR, RELAY_OFF);
        // Nyalakan Mosfet agar arus mengalir
        digitalWrite(Cfg::PIN_FAN_POWER, MOSFET_ON);
      }
      break;

    case STATE_INTAKE:
      if (now - _timer >= Cfg::FAN_INTAKE_MS) {
        _state = STATE_DELAY_READ;
        _timer = now;
        // Matikan Mosfet (memutus Ground, kipas mati total)
        digitalWrite(Cfg::PIN_FAN_POWER, MOSFET_OFF);
      }
      break;

    case STATE_DELAY_READ:
      if (now - _timer >= Cfg::FAN_DELAY_MS) {
        _state = STATE_EXHAUST;
        _timer = now;
        // NO = Fan Buang (Exhaust), Relay harus diaktifkan!
        digitalWrite(Cfg::PIN_FAN_DIR, RELAY_ON);
        // Nyalakan Mosfet agar arus mengalir kembali
        digitalWrite(Cfg::PIN_FAN_POWER, MOSFET_ON);
      }
      break;

    case STATE_EXHAUST:
      if (now - _timer >= Cfg::FAN_EXHAUST_MS) {
        _state = STATE_IDLE;
        _timer = now;
        // Matikan Mosfet (memutus Ground)
        digitalWrite(Cfg::PIN_FAN_POWER, MOSFET_OFF);
        // Lepas juga relay arah agar koil tidak panas
        digitalWrite(Cfg::PIN_FAN_DIR, RELAY_OFF);
      }
      break;
  }
}
