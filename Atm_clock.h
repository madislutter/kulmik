#pragma once

#include <Automaton.h>

using tick_cb = void (*) ();

class Atm_clock: public Machine {
 public:
  enum { OFF, ON }; // STATES
  enum { EVT_ON, EVT_OFF, EVT_TICK, ELSE }; // EVENTS
  Atm_clock(void) : Machine() {};
  Atm_clock& begin(void);
  Atm_clock& trace(Stream & stream);
  Atm_clock& on(void);
  Atm_clock& off(void);
  Atm_clock& onTick(tick_cb);
 private:
  enum { ENT_ON }; // ACTIONS
  int event(int id);
  void action(int id);
  atm_timer_millis timer_tick;
  tick_cb tick;
};
