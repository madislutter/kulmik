#pragma once

#include <Automaton.h>
#include "Abiheader.h"

class Atm_closed: public Machine {
  public:
    enum { IDLE, SALDO, EXIT }; // STATES
    enum { EVT_LEFT, EVT_TIMEOUT, ELSE }; // EVENTS
    Atm_closed(void) : Machine() {};
    Atm_closed& begin(void);
    Atm_closed& trace(Stream & stream);
    Atm_closed& nfc(Liige l);
    Atm_closed& reset();
    Atm_closed& onDraw(draw_cb);
    Atm_closed& onDrawF(draw_F_cb);
    Atm_closed& onClearScreen(clear_cb);
    Atm_closed& onExitToAdmin(Machine& machine, int event = 0);
    Atm_closed& onExitToAdmin(atm_cb_push_t callback, int idx = 0);
    Atm_closed& onTempOff(Machine& machine, int event = 0);
    Atm_closed& onTempOff(atm_cb_push_t callback, int idx = 0);
    Atm_closed& onTempOn(Machine& machine, int event = 0);
    Atm_closed& onTempOn(atm_cb_push_t callback, int idx = 0);
    Atm_closed& onClockOn(Machine& machine, int event = 0);
    Atm_closed& onClockOn(atm_cb_push_t callback, int idx = 0);
    Atm_closed& onClockOff(Machine& machine, int event = 0);
    Atm_closed& onClockOff(atm_cb_push_t callback, int idx = 0);
    Atm_closed& onNFCon(Machine& machine, int event = 0);
    Atm_closed& onNFCon(atm_cb_push_t callback, int idx = 0);
    Atm_closed& onNFCoff(Machine& machine, int event = 0);
    Atm_closed& onNFCoff(atm_cb_push_t callback, int idx = 0);
    Atm_closed& left(void);
    Liige l;
  private:
    enum { ENT_IDLE, ENT_SALDO }; // ACTIONS
    enum { ON_EXIT_ADMIN, ON_TEMP_OFF, ON_TEMP_ON, ON_CLOCK_ON, ON_CLOCK_OFF, ON_NFC_ON, ON_NFC_OFF, CONN_MAX }; // CONNECTORS
    atm_connector connectors[CONN_MAX];
    atm_timer_millis timer_timeout;
    clear_cb clearScreen;
    draw_cb draw;
    draw_F_cb draw_F;
    int event(int id);
    void action(int id);
};
