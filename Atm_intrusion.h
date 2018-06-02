#pragma once

#include <Automaton.h>
#include "Abiheader.h"

class Atm_intrusion: public Machine {
  public:
    enum { IDLE, KNOWN, UNKNOWN, IDENTIFYING, EXIT }; // STATES
    enum { EVT_INTRUSION_OVER, EVT_TIMEOUT, ELSE }; // EVENTS
    Atm_intrusion(void) : Machine() {};
    Atm_intrusion& begin(void);
    Atm_intrusion& trace(Stream & stream);
    Atm_intrusion& onClearScreen(clear_cb);
    Atm_intrusion& onDraw(draw_cb);
    Atm_intrusion& onDrawF(draw_F_cb);
    Atm_intrusion& onExit(Machine& machine, int event = 0);
    Atm_intrusion& onExit(atm_cb_push_t callback, int idx = 0);
    Atm_intrusion& onSirenOff(Machine& machine, int event = 0);
    Atm_intrusion& onSirenOff(atm_cb_push_t callback, int idx = 0);
    Atm_intrusion& onSirenOn(Machine& machine, int event = 0);
    Atm_intrusion& onSirenOn(atm_cb_push_t callback, int idx = 0);
    Atm_intrusion& onNFCon(Machine& machine, int event = 0);
    Atm_intrusion& onNFCon(atm_cb_push_t callback, int idx = 0);
    Atm_intrusion& onNFCoff(Machine& machine, int event = 0);
    Atm_intrusion& onNFCoff(atm_cb_push_t callback, int idx = 0);
    Atm_intrusion& onSalvestaSissetungija(salvesta_liige_cb);
    Atm_intrusion& intrusion_detected(void);
    Atm_intrusion& intrusion_detected(Liige l);
    Atm_intrusion& intrusion_over(void);
    Atm_intrusion& nfc(Liige l);
    Atm_intrusion& nfc_unknown(const char* uid);
  private:
    enum { ENT_IDLE, ENT_KNOWN, ENT_UNKNOWN, ENT_IDENTIFYING, ENT_EXIT }; // ACTIONS
    enum { ON_EXIT, ON_SIREN_OFF, ON_SIREN_ON, ON_NFC_ON, ON_NFC_OFF, CONN_MAX }; // CONNECTORS
    atm_connector connectors[CONN_MAX];
    atm_timer_millis timer_timeout;
    clear_cb clearScreen;
    draw_cb draw;
    draw_F_cb draw_F;
    salvesta_liige_cb salvestaSissetungija;
    Liige l;
    int event(int id);
    void action(int id);
};
