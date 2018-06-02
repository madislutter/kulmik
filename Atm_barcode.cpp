#include "Atm_barcode.h"

Atm_barcode& Atm_barcode::begin(byte DATA, byte CLK, char* BUFFER, int BUFFER_LEN) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*              ON_ENTER    ON_LOOP       ON_EXIT     EVT_ON   EVT_OFF  EVT_SCANNED  ELSE */
    /*  SLEEPING */       -1, ATM_SLEEP, EXT_SLEEPING, LISTENING,       -1,          -1,   -1,
    /* LISTENING */       -1,        -1,           -1,        -1, SLEEPING,   LISTENING,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  skanner.begin(DATA, CLK);
  this->BUFFER = BUFFER;
  this->BUFFER_LEN = BUFFER_LEN;
  for (byte i = 0; i < BUFFER_LEN; i++)
    BUFFER[i] = '\0';

  return *this;
}

int Atm_barcode::event(int id) {
  switch (id) {
    case EVT_SCANNED:
      bool uusTriipkoodOnSkannitudSelKorral = false;
      while (skanner.available()) {
        if (uusTriipkoodOnSkannitud) reset();
        char c = skanner.read();
        if (c == PS2_ENTER) {
          uusTriipkoodOnSkannitud = true;
          uusTriipkoodOnSkannitudSelKorral = true;
          emptySkanner();
        } else {
          BUFFER[pointer++] = c;
          if (pointer == BUFFER_LEN) {// Triipkood on pikem kui puhver
            reset();
          }
        }
      }
      if (uusTriipkoodOnSkannitudSelKorral)
        push(connectors, ON_SCANNED, 0, 0, 0);
      return uusTriipkoodOnSkannitudSelKorral;
  }
  return 0;
}

void Atm_barcode::action(int id) {
  switch (id) {
    case EXT_SLEEPING:
      reset();
      while (skanner.available()) skanner.read();
      return;
  }
}

Atm_barcode& Atm_barcode::on() {
  trigger(EVT_ON);
  return *this;
}

Atm_barcode& Atm_barcode::off() {
  trigger(EVT_OFF);
  return *this;
}

Atm_barcode& Atm_barcode::onScanned(Machine& machine, int event) {
  onPush(connectors, ON_SCANNED, 0, 1, 1, machine, event);
  return *this;
}

Atm_barcode& Atm_barcode::onScanned(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_SCANNED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_barcode& Atm_barcode::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "BARCODE\0EVT_ON\0EVT_OFF\0EVT_SCANNED\0ELSE\0SLEEPING\0LISTENING");
  return *this;
}

void Atm_barcode::reset() {
  for (byte i = 0; i < pointer; i++)
    BUFFER[i] = '\0';
  pointer = 0;
  uusTriipkoodOnSkannitud = false;
}

void Atm_barcode::emptySkanner() {
  while (skanner.available()) skanner.read();
}
                                                          
