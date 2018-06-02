#pragma once

#include <Automaton.h>
#include <PS2Keyboard.h>
#include "Abiheader.h"

class Atm_barcode: public Machine {

 public:
  enum { SLEEPING, LISTENING }; // STATES
  enum { EVT_ON, EVT_OFF, EVT_SCANNED, ELSE }; // EVENTS
  Atm_barcode(void) : Machine() {};
  Atm_barcode& begin(byte, byte, char*, int);
  Atm_barcode& trace(Stream & stream);
  Atm_barcode& onScanned(Machine& machine, int event = 0);
  Atm_barcode& onScanned(atm_cb_push_t callback, int idx = 0);
  Atm_barcode& on(void);
  Atm_barcode& off(void);

 private:
  enum { EXT_SLEEPING }; // ACTIONS
  enum { ON_SCANNED, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  PS2Keyboard skanner;
  char* BUFFER;
  int BUFFER_LEN;
  byte pointer = 0;
  bool uusTriipkoodOnSkannitud = false;
  int event(int id);
  void action(int id);
  void reset();
  void emptySkanner();
};
