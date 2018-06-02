#pragma once

#include <Automaton.h>

class Atm_door: public Machine {
 public:
  enum { CLOSED, OPEN, OPENING, CLOSING }; // STATES
  enum { EVT_OPENED, EVT_CLOSED, EVT_DELAY, ELSE }; // EVENTS
  Atm_door(void) : Machine() {};
  Atm_door& begin(void);
  Atm_door& trace(Stream & stream);
  Atm_door& onClosed(Machine& machine, int event = 0);
  Atm_door& onClosed(atm_cb_push_t callback, int idx = 0);
  Atm_door& onOpened(Machine& machine, int event = 0);
  Atm_door& onOpened(atm_cb_push_t callback, int idx = 0);
  void opened(void);
  void closed(void);
 private:
  enum { ENT_OPEN, ENT_CLOSED }; // ACTIONS
  enum { ON_CLOSED, ON_OPENED, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  atm_timer_millis timer_delay;
  int event(int id);
  void action(int id);
};