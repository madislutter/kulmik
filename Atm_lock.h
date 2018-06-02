#pragma once

#include <Automaton.h>

class Atm_lock: public Machine {

 public:
  enum { LOCKED, UNLOCKED_OPEN, UNLOCKED_CLOSED, ALARM }; // STATES
  enum { EVT_UNLOCK, EVT_LOCK, EVT_OPENED, EVT_CLOSED, EVT_DELAY, ELSE }; // EVENTS
  Atm_lock(void) : Machine() {};
  Atm_lock& begin();
  Atm_lock& trace(Stream & stream);
  Atm_lock& onIntrusionDetected(Machine& machine, int event = 0);
  Atm_lock& onIntrusionDetected(atm_cb_push_t callback, int idx = 0);
  Atm_lock& onIntrusionOver(Machine& machine, int event = 0);
  Atm_lock& onIntrusionOver(atm_cb_push_t callback, int idx = 0);
  Atm_lock& onLocked(Machine& machine, int event = 0);
  Atm_lock& onLocked(atm_cb_push_t callback, int idx = 0);
  Atm_lock& onUnlocked(Machine& machine, int event = 0);
  Atm_lock& onUnlocked(atm_cb_push_t callback, int idx = 0);
  Atm_lock& lockingDelay(int delay);
  void lock(void);
  void unlock(void);
  void opened(void);
  void closed(void);

 private:
  enum { ENT_LOCKED, ENT_UNLOCKED_OPEN, ENT_UNLOCKED_CLOSED, ENT_ALARM, EXT_ALARM }; // ACTIONS
  enum { ON_INTRUSION_DETECTED, ON_INTRUSION_OVER, ON_LOCKED, ON_UNLOCKED, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  atm_timer_millis locking_delay;
  int event(int id);
  void action(int id);
};
