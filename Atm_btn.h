#pragma once

#include <Automaton.h>

class Atm_btn: public Machine {
  public:
    enum { NOT_PRESSED, WAITING, PRESSED }; // STATES
    enum { EVT_PRESSED, EVT_RELEASED, EVT_DEBOUNCE, ELSE }; // EVENTS
    Atm_btn(void) : Machine() {};
    Atm_btn& begin(void);
    Atm_btn& trace(Stream & stream);
    Atm_btn& onPressed(Machine& machine, int event = 0);
    Atm_btn& onPressed(atm_cb_push_t callback, int idx = 0);
    void pressed(void);
    void released(void);
  private:
    enum { ENT_PRESSED }; // ACTIONS
    enum { ON_PRESSED, CONN_MAX }; // CONNECTORS
    atm_connector connectors[CONN_MAX];
    atm_timer_millis timer_debounce;
    int event(int id);
    void action(int id);
};