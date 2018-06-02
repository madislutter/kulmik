  #pragma once

#include <Automaton.h>
#include "Atm_open.h"
#include "Atm_closed.h"
#include "Atm_intrusion.h"
#include "Atm_admin.h"
#include "Abiheader.h"

using ava_cb = void (*) ();
using sulge_cb = void (*) ();
using bool_cb = bool (*) ();

class Atm_fridge: public Machine {
 public:
  enum { START, NO_SD, OPEN, CLOSED, ADMIN, UNKNOWN_NFC, INTRUSION }; // STATES
  enum { EVT_INTRUSION_DETECTED, EVT_SD_INSERTED, EVT_SD_EJECTED, EVT_MSG_TIMEOUT, ELSE }; // EVENTS
  Atm_fridge(void) : Machine() {};
  Atm_fridge& begin(Atm_open, Atm_closed, Atm_admin, Atm_intrusion);
  Atm_fridge& trace(Stream & stream);
  bool isBusy();

  Atm_fridge& onClearScreen(clear_cb);
  Atm_fridge& onDraw(draw_cb);
  Atm_fridge& onDrawF(draw_F_cb);
  Atm_fridge& onLock(Machine& machine, int event = 0);
  Atm_fridge& onLock(atm_cb_push_t callback, int idx = 0);
  Atm_fridge& onTempOff(Machine& machine, int event = 0);
  Atm_fridge& onTempOff(atm_cb_push_t callback, int idx = 0);
  Atm_fridge& onTempOn(Machine& machine, int event = 0);
  Atm_fridge& onTempOn(atm_cb_push_t callback, int idx = 0);
  Atm_fridge& onClockOn(Machine& machine, int event = 0);
  Atm_fridge& onClockOn(atm_cb_push_t callback, int idx = 0);
  Atm_fridge& onClockOff(Machine& machine, int event = 0);
  Atm_fridge& onClockOff(atm_cb_push_t callback, int idx = 0);
  Atm_fridge& onUnlockChest(Machine& machine, int event = 0);
  Atm_fridge& onUnlockChest(atm_cb_push_t callback, int idx = 0);
  Atm_fridge& onUnlockFridge(Machine& machine, int event = 0);
  Atm_fridge& onUnlockFridge(atm_cb_push_t callback, int idx = 0);
  Atm_fridge& onOlemeSuletud(bool_cb);
  Atm_fridge& onSulgeTehinguteks(sulge_cb);
  Atm_fridge& onAvaTehinguteks(ava_cb);
  Atm_fridge& up();
  Atm_fridge& down();
  Atm_fridge& left();
  Atm_fridge& right();
  Atm_fridge& nfc(Liige l);
  Atm_fridge& nfc_unknown(const char* uid);
  Atm_fridge& nfc_locked();
  Atm_fridge& barcode(Toode t);
  Atm_fridge& barcode_unknown(char* kood);
  Atm_fridge& door_opens();
  Atm_fridge& doors_closed();
  Atm_fridge& intrusion_detected();
  Atm_fridge& intrusion_over();
  void updateTemp(float, float);
  Atm_fridge& sd_inserted();
  Atm_fridge& sd_ejected();
  Atm_fridge& exitAdminClosed();
  Atm_fridge& exitAdminOpen();
  Atm_fridge& exitIntrusion();
  Atm_fridge& enterAdmin();
  void valitiLiige(Liige &l);
  void valitiToode(Toode &t);
  void cancelSelect();

 private:
  enum { ENT_START, ENT_NO_SD, ENT_OPEN, LP_OPEN, ENT_CLOSED, LP_CLOSED, ENT_ADMIN, LP_ADMIN, EXT_ADMIN, ENT_UNKNOWN_NFC, ENT_INTRUSION, LP_INTRUSION, EXT_INTRUSION }; // ACTIONS
  enum { ON_LOCK, ON_TEMP_OFF, ON_TEMP_ON, ON_CLOCK_ON, ON_CLOCK_OFF, ON_UNLOCK_CHEST, ON_UNLOCK_FRIDGE, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  int event(int id);
  void action(int id);
  atm_timer_millis timer_msg_timeout;
  clear_cb clearScreen;
  draw_cb draw;
  draw_F_cb draw_F;
  bool_cb olemeSuletud;
  ava_cb avaTehinguteks;
  sulge_cb sulgeTehinguteks;
  Atm_open open;
  Atm_closed closed;
  Atm_admin admin;
  Atm_intrusion intrusion;
};
