#pragma once

#include <Automaton.h>

typedef void (*update_cb) (float temp, float hum);

class Atm_temperature: public Machine {

 public:
  enum { SLEEP, READING }; // STATES
  enum { EVT_ON, EVT_OFF, EVT_DELAY, ELSE }; // EVENTS
  Atm_temperature(void) : Machine() {};
  Atm_temperature& begin(void);
  Atm_temperature& trace(Stream & stream);
  Atm_temperature& onUpdate(update_cb);
  Atm_temperature& on(void);
  Atm_temperature& off(void);
  Atm_temperature& setInterval(unsigned long interval);

 private:
  enum { ENT_READING }; // ACTIONS
  enum { ON_UPDATE, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  atm_timer_millis timer_delay;
  update_cb update;
  int event(int id); 
  void action(int id); 
};
