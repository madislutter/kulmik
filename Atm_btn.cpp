#include "Atm_btn.h"

Atm_btn& Atm_btn::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                   ON_ENTER  ON_LOOP  ON_EXIT  EVT_PRESSED  EVT_RELEASED  EVT_DEBOUNCE  ELSE */
    /* NOT_PRESSED */          -1,      -1,      -1,     WAITING,           -1,           -1,   -1,
    /*     WAITING */          -1,      -1,      -1,          -1,  NOT_PRESSED,      PRESSED,   -1,
    /*     PRESSED */ ENT_PRESSED,      -1,      -1,          -1,  NOT_PRESSED,           -1,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  timer_debounce.set(5);
  return *this;
}

int Atm_btn::event(int id) {
  switch (id) {
    case EVT_DEBOUNCE:
      return timer_debounce.expired(this);
  }
  return 0;
}

void Atm_btn::action(int id) {
  switch (id) {
    case ENT_PRESSED:
      push(connectors, ON_PRESSED, 0, 0, 0);
      return;
  }
}

void Atm_btn::pressed() {
  trigger(EVT_PRESSED);
}

void Atm_btn::released() {
  trigger(EVT_RELEASED);
}

Atm_btn& Atm_btn::onPressed(Machine& machine, int event) {
  onPush(connectors, ON_PRESSED, 0, 1, 1, machine, event);
  return *this;
}

Atm_btn& Atm_btn::onPressed(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_PRESSED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_btn& Atm_btn::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "BTN\0EVT_PRESSED\0EVT_RELEASED\0EVT_DEBOUNCE\0ELSE\0NOT_PRESSED\0WAITING\0PRESSED");
  return *this;
}
