#include "Atm_closed.h"

Atm_closed& Atm_closed::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*           ON_ENTER  ON_LOOP   ON_EXIT  EVT_LEFT  EVT_TIMEOUT  ELSE */
    /*  IDLE */  ENT_IDLE,      -1,       -1,       -1,          -1,   -1,
    /* SALDO */ ENT_SALDO,      -1,       -1,     IDLE,        IDLE,   -1,
    /*  EXIT */        -1,      -1,       -1,       -1,          -1,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  timer_timeout.set(10000); // 10s
  return *this;
}

int Atm_closed::event(int id) {
  switch (id) {
    case EVT_TIMEOUT:
      return timer_timeout.expired(this);
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push(connectors, ON_EXITADMIN, 0, <v>, <up>);
 *   push(connectors, ON_TEMPOFF, 0, <v>, <up>);
 *   push(connectors, ON_TEMPON, 0, <v>, <up>);
 *   push(connectors, ON_CLOCK_ON, 0, <v>, <up>);
 *   push(connectors, ON_CLOCK_OFF, 0, <v>, <up>);
 *   push(connectors, ON_NFC_ON, 0, <v>, <up>);
 *   push(connectors, ON_NFC_OFF, 0, <v>, <up>);
 */

void Atm_closed::action(int id) {
  switch (id) {
    case ENT_IDLE:
      clearScreen();
      draw_F(0, F("   Oleme suletud!"));
      push(connectors, ON_TEMP_ON, 0, 0, 0);
      push(connectors, ON_CLOCK_ON, 0, 0, 0);
      push(connectors, ON_NFC_ON, 0, 0, 0);
      this->l = {};
      return;
    case ENT_SALDO:
      push(connectors, ON_TEMP_OFF, 0, 0, 0);
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      clearScreen();
      char txt[21];
      txt[0] = '\0';
      strcat(txt, "Tere, ");
      strncat(txt, this->l.nimi.c_str(), 14);
      draw(0, txt);

      txt[0] = '\0';
      strcat(txt, "Saldo: ");
      dtostrf(this->l.saldo / 100.0, 6, 2, txt + 7);
      strcat(txt, "€");
      draw(1, txt);

      draw_F(2, F("Osta ei saa. Oleme"));
      draw_F(3, F("suletud!"));
      return;
  }
}

Atm_closed& Atm_closed::nfc(Liige l) {
  if (l.isAdmin) {
    push(connectors, ON_EXIT_ADMIN, 0, 0, 0);
    state(EXIT);
  } else if (strcmp(this->l.nimi.c_str(), l.nimi.c_str()) != 0) {
    this->l = l;
    state(SALDO);
  } else {
    this->state_millis = millis();
  }
  return *this;
}

Atm_closed& Atm_closed::reset() {
  state(IDLE);
  return *this;
}

Atm_closed& Atm_closed::left() {
  trigger(EVT_LEFT);
  return *this;
}

Atm_closed& Atm_closed::onClearScreen(clear_cb clearScreen) {
  this->clearScreen = clearScreen;
  return *this;
}

Atm_closed& Atm_closed::onDraw(draw_cb draw) {
  this->draw = draw;
  return *this;
}

Atm_closed& Atm_closed::onDrawF(draw_F_cb draw_F) {
  this->draw_F = draw_F;
  return *this;
}

Atm_closed& Atm_closed::onExitToAdmin(Machine& machine, int event) {
  onPush(connectors, ON_EXIT_ADMIN, 0, 1, 1, machine, event);
  return *this;
}

Atm_closed& Atm_closed::onExitToAdmin(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_EXIT_ADMIN, 0, 1, 1, callback, idx);
  return *this;
}

Atm_closed& Atm_closed::onTempOff(Machine& machine, int event) {
  onPush(connectors, ON_TEMP_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_closed& Atm_closed::onTempOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_TEMP_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_closed& Atm_closed::onTempOn(Machine& machine, int event) {
  onPush(connectors, ON_TEMP_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_closed& Atm_closed::onTempOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_TEMP_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_closed& Atm_closed::onClockOn(Machine& machine, int event) {
  onPush(connectors, ON_CLOCK_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_closed& Atm_closed::onClockOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_CLOCK_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_closed& Atm_closed::onClockOff(Machine& machine, int event) {
  onPush(connectors, ON_CLOCK_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_closed& Atm_closed::onClockOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_CLOCK_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_closed& Atm_closed::onNFCon(Machine& machine, int event) {
  onPush(connectors, ON_NFC_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_closed& Atm_closed::onNFCon(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_NFC_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_closed& Atm_closed::onNFCoff(Machine& machine, int event) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_closed& Atm_closed::onNFCoff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_closed& Atm_closed::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "CLOSED\0EVT_LEFT\0EVT_TIMEOUT\0ELSE\0IDLE\0SALDO\0EXIT");
  return *this;
}
