#include "Atm_intrusion.h"

Atm_intrusion& Atm_intrusion::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                       ON_ENTER  ON_LOOP  ON_EXIT  EVT_INTRUSION_OVER   EVT_TIMEOUT   ELSE */
    /*       IDLE  */        ENT_IDLE,      -1,      -1,                 -1,           -1,    -1,
    /*       KNOWN */       ENT_KNOWN,      -1,      -1,               EXIT,           -1,    -1,
    /*     UNKNOWN */     ENT_UNKNOWN,      -1,      -1,        IDENTIFYING,           -1,    -1,
    /* IDENTIFYING */ ENT_IDENTIFYING,      -1,      -1,                 -1,         EXIT,    -1,
    /*        EXIT */        ENT_EXIT,      -1,      -1,                 -1,           -1,  IDLE,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  timer_timeout.set(120000); // 2min
  this->l = {};
  return *this;
}

int Atm_intrusion::event(int id) {
  switch (id) {
    case EVT_TIMEOUT:
      return timer_timeout.expired(this);
  }
  return 0;
}

/* Add C++ code for each action
   This generates the 'output' for the state machine

   Available connectors:
     push(connectors, ON_EXIT, 0, <v>, <up>);
     push(connectors, ON_SIREN_OFF, 0, <v>, <up>);
     push(connectors, ON_SIREN_ON, 0, <v>, <up>);
*/

void Atm_intrusion::action(int id) {
  switch (id) {
    case ENT_IDLE:
      return;
    case ENT_KNOWN:
      push(connectors, ON_SIREN_ON, 0, 0, 0);
      clearScreen();
      char txt[21];
      txt[0] = '\0';
      strcat(txt, "Hei, ");
      strncat(txt, this->l.nimi.c_str(), 15);
      draw(0, txt);
      draw_F(1, F("Lõpeta kohe ära!"));
      salvestaSissetungija(this->l);
      return;
    case ENT_UNKNOWN:
      push(connectors, ON_SIREN_ON, 0, 0, 0);
      push(connectors, ON_NFC_ON, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Hei, kleptomaan!"));
      draw_F(1, F("Lõpeta kohe ära!"));
      return;
    case ENT_IDENTIFYING:
      clearScreen();
      draw_F(0, F("Tuvasta end sireeni"));
      draw_F(1, F("vaigistamiseks!"));
      return;
    case ENT_EXIT:
      this->l = {};
      push(connectors, ON_SIREN_OFF, 0, 0, 0);
      push(connectors, ON_EXIT, 0, 0, 0);
      return;
  }
}

Atm_intrusion& Atm_intrusion::intrusion_detected() {
  if (strcmp(this->l.nimi.c_str(), "") == 0) {
    state(UNKNOWN);
  } else {
    state(KNOWN);
  }
  return *this;
}

Atm_intrusion& Atm_intrusion::intrusion_detected(Liige l) {
  this->l = l;
  state(KNOWN);
  return *this;
}

Atm_intrusion& Atm_intrusion::intrusion_over() {
  trigger(EVT_INTRUSION_OVER);
  return *this;
}

Atm_intrusion& Atm_intrusion::nfc(Liige l) {
  switch (state()) {
    case KNOWN:
    case UNKNOWN:
      this->l = l;
      state(KNOWN);
      break;
    case IDENTIFYING:
      salvestaSissetungija(l);
      state(EXIT);
      break;
  }

  return *this;
}

Atm_intrusion& Atm_intrusion::nfc_unknown(const char* uid) {
  Liige l = {"", 0, uid, 0};
  switch (state()) {
    case KNOWN:
    case UNKNOWN:
      this->l = l;
      state(KNOWN);
      break;
    case IDENTIFYING:
      salvestaSissetungija(l);
      state(EXIT);
      break;
  }

  return *this;
}

Atm_intrusion& Atm_intrusion::onClearScreen(clear_cb clearScreen) {
  this->clearScreen = clearScreen;
  return *this;
}

Atm_intrusion& Atm_intrusion::onDraw(draw_cb draw) {
  this->draw = draw;
  return *this;
}

Atm_intrusion& Atm_intrusion::onDrawF(draw_F_cb draw_F) {
  this->draw_F = draw_F;
  return *this;
}

Atm_intrusion& Atm_intrusion::onSalvestaSissetungija(salvesta_liige_cb salvestaSissetungija) {
  this->salvestaSissetungija = salvestaSissetungija;
  return *this;
}

Atm_intrusion& Atm_intrusion::onExit(Machine& machine, int event) {
  onPush(connectors, ON_EXIT, 0, 1, 1, machine, event);
  return *this;
}

Atm_intrusion& Atm_intrusion::onExit(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_EXIT, 0, 1, 1, callback, idx);
  return *this;
}

Atm_intrusion& Atm_intrusion::onSirenOff(Machine& machine, int event) {
  onPush(connectors, ON_SIREN_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_intrusion& Atm_intrusion::onSirenOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_SIREN_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_intrusion& Atm_intrusion::onSirenOn(Machine& machine, int event) {
  onPush(connectors, ON_SIREN_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_intrusion& Atm_intrusion::onSirenOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_SIREN_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_intrusion& Atm_intrusion::onNFCon(Machine& machine, int event) {
  onPush(connectors, ON_NFC_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_intrusion& Atm_intrusion::onNFCon(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_NFC_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_intrusion& Atm_intrusion::onNFCoff(Machine& machine, int event) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_intrusion& Atm_intrusion::onNFCoff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_intrusion& Atm_intrusion::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
                    "INTRUSION\0EVT_INTRUSION_OVER\0EVT_TIMEOUT\0ELSE\0IDLE\0KNOWN\0UNKNOWN\0IDENTIFYING\0EXIT");
  return *this;
}
