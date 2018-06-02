#pragma once

#include <Automaton.h>
#include "Abiheader.h"

class Atm_open: public Machine {
  public:
    enum { IDLE, SALDO_LOCKED, SALDO_UNLOCKED, TAKE_PRODUCT, SCAN_PRODUCT, SCAN_PRODUCT_LONG, DEAL, NO_DEAL, NO_MORE_DEAL, RETURN_PRODUCT, UNKNOWN_PRODUCT, UNKNOWN_PRODUCT_RETRY, EXIT }; // STATES
    enum { EVT_DOOR_OPENED, EVT_DOORS_CLOSED, EVT_TIMEOUT, EVT_WAITFORSCAN, ELSE }; // EVENTS
    Atm_open(void) : Machine() {};
    Atm_open& begin(void);
    Atm_open& trace(Stream & stream);
    Atm_open& onBeepThreeTimes(clear_cb);
    Atm_open& onClearScreen(clear_cb);
    Atm_open& onDraw(draw_cb);
    Atm_open& onDrawF(draw_F_cb);
    Atm_open& onSalvestaLiige(salvesta_liige_cb);
    Atm_open& onSalvestaTehing(salvesta_tehing_cb);
    Atm_open& onBarcodeOff(Machine& machine, int event = 0);
    Atm_open& onBarcodeOff(atm_cb_push_t callback, int idx = 0);
    Atm_open& onBarcodeOn(Machine& machine, int event = 0);
    Atm_open& onBarcodeOn(atm_cb_push_t callback, int idx = 0);
    Atm_open& onExitToAdmin(Machine& machine, int event = 0);
    Atm_open& onExitToAdmin(atm_cb_push_t callback, int idx = 0);
    Atm_open& onLock(Machine& machine, int event = 0);
    Atm_open& onLock(atm_cb_push_t callback, int idx = 0);
    Atm_open& onNFCoff(Machine& machine, int event = 0);
    Atm_open& onNFCoff(atm_cb_push_t callback, int idx = 0);
    Atm_open& onNFCon(Machine& machine, int event = 0);
    Atm_open& onNFCon(atm_cb_push_t callback, int idx = 0);
    Atm_open& onTempOff(Machine& machine, int event = 0);
    Atm_open& onTempOff(atm_cb_push_t callback, int idx = 0);
    Atm_open& onTempOn(Machine& machine, int event = 0);
    Atm_open& onTempOn(atm_cb_push_t callback, int idx = 0);
    Atm_open& onClockOn(Machine& machine, int event = 0);
    Atm_open& onClockOn(atm_cb_push_t callback, int idx = 0);
    Atm_open& onClockOff(Machine& machine, int event = 0);
    Atm_open& onClockOff(atm_cb_push_t callback, int idx = 0);
    Atm_open& onUnlockFridge(Machine& machine, int event = 0);
    Atm_open& onUnlockFridge(atm_cb_push_t callback, int idx = 0);
    Atm_open& onBeepingOn(Machine& machine, int event = 0);
    Atm_open& onBeepingOn(atm_cb_push_t callback, int idx = 0);
    Atm_open& onBeepingOff(Machine& machine, int event = 0);
    Atm_open& onBeepingOff(atm_cb_push_t callback, int idx = 0);
    void nfc(Liige l);
    void barcode(Toode t);
    void barcode_unknown(char* kood);
    void up();
    void down();
    void left();
    void right();
    void door_opened();
    void doors_closed();
    void reset();
    Liige l;
  private:
    enum { ENT_IDLE, EXT_IDLE, ENT_SALDO_LOCKED, ENT_SALDO_UNLOCKED, ENT_TAKE_PRODUCT, ENT_SCAN_PRODUCT, ENT_SCAN_PRODUCT_LONG, EXT_SCAN_PRODUCT_LONG, ENT_DEAL, ENT_NO_DEAL, ENT_NO_MORE_DEAL, ENT_RETURN_PRODUCT, ENT_UNKNOWN_PRODUCT, ENT_UNKNOWN_PRODUCT_RETRY, ENT_EXIT }; // ACTIONS
    enum { ON_BARCODE_OFF, ON_BARCODE_ON, ON_EXIT_ADMIN, ON_LOCK, ON_NFC_OFF, ON_NFC_ON, ON_TEMP_OFF, ON_TEMP_ON, ON_CLOCK_ON, ON_CLOCK_OFF, ON_UNLOCK_FRIDGE, ON_BEEPING_ON, ON_BEEPING_OFF, CONN_MAX }; // CONNECTORS
    atm_connector connectors[CONN_MAX];
    atm_timer_millis timer_timeout;
    atm_timer_millis timer_waitForScan;
    int nrOfWrongBarcodesScanned;
    Toode t;
    clear_cb beepThreeTimes;
    clear_cb clearScreen;
    draw_cb draw;
    draw_F_cb draw_F;
    salvesta_tehing_cb salvestaTehing;
    int event(int id);
    void action(int id);
};
