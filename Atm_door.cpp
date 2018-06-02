#include "Atm_door.h"

Atm_door& Atm_door::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*              ON_ENTER  ON_LOOP  ON_EXIT  EVT_OPENED  EVT_CLOSED  EVT_DELAY  ELSE */
    /*  CLOSED */ ENT_CLOSED,      -1,      -1,    OPENING,         -1,        -1,   -1,
    /*    OPEN */   ENT_OPEN,      -1,      -1,         -1,    CLOSING,        -1,   -1,
    /* OPENING */         -1,      -1,      -1,         -1,     CLOSED,      OPEN,   -1,
    /* CLOSING */         -1,      -1,      -1,       OPEN,         -1,    CLOSED,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  timer_delay.set(100); // 0,1s
  return *this;
}

int Atm_door::event(int id) {
  switch (id) {
    case EVT_DELAY:
      return timer_delay.expired(this);
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push(connectors, ON_CLOSED, 0, <v>, <up>);
 *   push(connectors, ON_OPENED, 0, <v>, <up>);
 */
void Atm_door::action(int id) {
  switch (id) {
    case ENT_CLOSED:
      push(connectors, ON_CLOSED, 0, 0, 0);
      return;
    case ENT_OPEN:
      push(connectors, ON_OPENED, 0, 0, 0);
      return;
  }
}

void Atm_door::opened() {
  trigger(EVT_OPENED);
}

void Atm_door::closed() {
  trigger(EVT_CLOSED);
}

Atm_door& Atm_door::onClosed(Machine& machine, int event) {
  onPush(connectors, ON_CLOSED, 0, 1, 1, machine, event);
  return *this;
}

Atm_door& Atm_door::onClosed(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_CLOSED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_door& Atm_door::onOpened(Machine& machine, int event) {
  onPush(connectors, ON_OPENED, 0, 1, 1, machine, event);
  return *this;
}

Atm_door& Atm_door::onOpened(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_OPENED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_door& Atm_door::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "DOOR\0EVT_OPENED\0EVT_CLOSED\0EVT_DELAY\0ELSE\0CLOSED\0OPEN\0OPENING\0CLOSING");
  return *this;
}
