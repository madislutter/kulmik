#pragma once

#include <Automaton.h>
#include "Abiheader.h"
#include <Menu.h>

class Atm_admin: public Machine {
  public:
    enum { MENU, HOIA_LAHTI, SULARAHA_OST, TAKE_PRODUCTS, SCAN_PRODUCTS, DEAL, UNKNOWN_PRODUCT, EXIT }; // STATES
    enum { EVT_MENUTIMEOUT, EVT_MSGTIMEOUT, EVT_DOOR_OPENED, EVT_DOORS_CLOSED, ELSE }; // EVENTS
    Atm_admin(void) : Machine() {};
    Atm_admin& begin(void);
    Atm_admin& trace(Stream & stream);
    Atm_admin& onClearScreen(clear_cb);
    Atm_admin& onDraw(draw_cb);
    Atm_admin& onDrawF(draw_F_cb);
    Atm_admin& onSalvestaTehing(salvesta_tehing_cb);
    Atm_admin& onExitClosed(Machine& machine, int event = 0);
    Atm_admin& onExitClosed(atm_cb_push_t callback, int idx = 0);
    Atm_admin& onExitOpen(Machine& machine, int event = 0);
    Atm_admin& onExitOpen(atm_cb_push_t callback, int idx = 0);
    Atm_admin& onExitYritus(Machine& machine, int event = 0);
    Atm_admin& onExitYritus(atm_cb_push_t callback, int idx = 0);
    Atm_admin& onLock(Machine& machine, int event = 0);
    Atm_admin& onLock(atm_cb_push_t callback, int idx = 0);
    Atm_admin& onUnlock(Machine& machine, int event = 0);
    Atm_admin& onUnlock(atm_cb_push_t callback, int idx = 0);
    Atm_admin& onNFCoff(Machine& machine, int event = 0);
    Atm_admin& onNFCoff(atm_cb_push_t callback, int idx = 0);
    void up();
    void down();
    void left();
    void right();
    void exitMenu();
    void reset();
    void doorOpened();
    void doorsClosed();
    void barcode(Toode t);
    void barcode_unknown(char* code);
  private:
    enum { ENT_MENU, LP_MENU, ENT_HOIA_LAHTI, EXT_HOIA_LAHTI, ENT_SULARAHA_OST, ENT_TAKE_PRODUCTS, ENT_SCAN_PRODUCTS, ENT_DEAL, ENT_UNKNOWN_PRODUCT, ENT_EXIT }; // ACTIONS
    enum { ON_EXIT_CLOSED, ON_EXIT_OPEN, ON_EXIT_YRITUS, ON_LOCK, ON_UNLOCK, ON_NFC_OFF, CONN_MAX }; // CONNECTORS
    atm_connector connectors[CONN_MAX];
    atm_timer_millis timer_menu_timeout, timer_msg_timeout;
    clear_cb clearScreen;
    draw_cb draw;
    draw_F_cb draw_F;
    salvesta_tehing_cb salvestaTehing;
    Toode t;
    Liige l;
    void showMenu();
    void kontrolliTegevust();
    int menuTegevus = 0;
    int UP = 1;
    int DOWN = 2;
    int LEFT = 3;
    int RIGHT = 4;
    int event(int id);
    void action(int id);
};
