#include "Atm_fridge.h"

Atm_fridge& Atm_fridge::begin(Atm_open open, Atm_closed closed, Atm_admin admin, Atm_intrusion intrusion) { 
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                       ON_ENTER       ON_LOOP        ON_EXIT  EVT_INTRUSION_DETECTED  EVT_SD_INSERTED  EVT_SD_EJECTED  EVT_MSG_TIMEOUT  ELSE */
    /*       START */       ENT_START,           -1,            -1,                     -1,              -1,          NO_SD,              -1,   -1,
    /*       NO_SD */       ENT_NO_SD,           -1,            -1,                     -1,           START,             -1,              -1,   -1,
    /*        OPEN */        ENT_OPEN,      LP_OPEN,            -1,              INTRUSION,              -1,          NO_SD,              -1,   -1,
    /*      CLOSED */      ENT_CLOSED,    LP_CLOSED,            -1,              INTRUSION,              -1,          NO_SD,              -1,   -1,
    /*       ADMIN */       ENT_ADMIN,     LP_ADMIN,     EXT_ADMIN,                     -1,              -1,             -1,              -1,   -1,
    /* UNKNOWN_NFC */ ENT_UNKNOWN_NFC,           -1,            -1,              INTRUSION,              -1,             -1,           START,   -1,
    /*   INTRUSION */   ENT_INTRUSION, LP_INTRUSION, EXT_INTRUSION,              INTRUSION,              -1,             -1,              -1,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);

  timer_msg_timeout.set(10000); // 10s
  this->open = open.begin();
  this->closed = closed.begin();
  this->admin = admin.begin();
  this->intrusion = intrusion;
  this->intrusion.begin().cycle().cycle().cycle();

  return *this;          
}

int Atm_fridge::event(int id) {
  switch (id) {
    case EVT_MSG_TIMEOUT:
      return timer_msg_timeout.expired(this);
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push(connectors, ON_LOCK, 0, <v>, <up>);
 *   push(connectors, ON_TEMP_OFF, 0, <v>, <up>);
 *   push(connectors, ON_TEMP_ON, 0, <v>, <up>);
 *   push(connectors, ON_CLOCK_ON, 0, <v>, <up>);
 *   push(connectors, ON_CLOCK_OFF, 0, <v>, <up>);
 *   push(connectors, ON_UNLOCK_CHEST, 0, <v>, <up>);
 *   push(connectors, ON_UNLOCK_FRIDGE, 0, <v>, <up>);
 */

void Atm_fridge::action(int id) {
  switch (id) {
    case ENT_START:
      if (olemeSuletud()) {
        state(CLOSED);
      } else {
        state(OPEN);
      }
      return;
    case ENT_NO_SD:
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_CLOCK_ON, 0, 0, 0);
      clearScreen();
      draw_F(0, F("SD kaarti ei leitud"));
      push(connectors, ON_TEMP_ON, 0, 0, 0);
      return;
    case ENT_OPEN:
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      open.reset();
      return;
    case LP_OPEN:
      open.cycle();
      return;
    case ENT_CLOSED:
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      closed.reset();
      return;
    case LP_CLOSED:
      closed.cycle();
      return;
    case ENT_ADMIN:
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      push(connectors, ON_TEMP_OFF, 0, 0, 0);
      push(connectors, ON_LOCK, 0, 0, 0);
      admin.reset();
      return;
    case LP_ADMIN:
      admin.cycle();
      return;
    case EXT_ADMIN:
      push(connectors, ON_LOCK, 0, 0, 0);
      return;
    case ENT_UNKNOWN_NFC:
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_TEMP_OFF, 0, 0, 0);
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      return;
    case ENT_INTRUSION:
      push(connectors, ON_TEMP_OFF, 0, 0, 0);
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      //intrusion.reset();
      return;
    case LP_INTRUSION:
      intrusion.cycle();
      return;
    case EXT_INTRUSION:
      return;
  }
}

bool Atm_fridge::isBusy() {
  int currentState = state();
  if (currentState == OPEN || currentState == CLOSED)
    return false;
  return true;
}

Atm_fridge& Atm_fridge::up() {
  switch (state()) {
    case ADMIN:
      admin.up();
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::down() {
  switch (state()) {
    case ADMIN:
      admin.down();
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::left() {
  switch (state()) {
    case ADMIN:
      admin.left();
      break;
    case OPEN:
      open.left();
      break;
    case CLOSED:
      closed.left();
      break;
    case UNKNOWN_NFC:
      state(START);
  }
  return *this;
}

Atm_fridge& Atm_fridge::right() {
  switch (state()) {
    case ADMIN:
      admin.right();
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::nfc(Liige l) {
  switch (state()) {
    case OPEN:
      open.nfc(l);
      break;
    case CLOSED:
      closed.nfc(l);
      break;
    case INTRUSION:
      intrusion.nfc(l);
      break;
    case NO_SD:
    case UNKNOWN_NFC:
      if (olemeSuletud()) {
        state(CLOSED);
        closed.nfc(l);
      } else {
        state(OPEN);
        open.nfc(l);
      }
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::nfc_unknown(const char* uid) {
  switch (state()) {
    case INTRUSION:
      intrusion.nfc_unknown(uid);
      break;
    default:
      state(UNKNOWN_NFC);
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_TEMP_OFF, 0, 0, 0);
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Tundmatu kiipkaart:"));
      draw(1, uid);
      draw_F(2, F("Ütle oma kood joogi-"));
      draw_F(3, F("vanemale"));
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::nfc_locked() {
  switch (state()) {
    case INTRUSION:
      break;
    default:
      state(UNKNOWN_NFC);
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_TEMP_OFF, 0, 0, 0);
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Pane ekraan tööle võ"));
      draw_F(1, F("i paigalda äpp siit:"));
      draw_F(2, F("bit.ly/2rwg0ga"));
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::barcode(Toode t) {
  switch (state()) {
    case OPEN:
      open.barcode(t);
      break;
    case ADMIN:
      admin.barcode(t);
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::barcode_unknown(char* kood) {
  switch (state()) {
    case OPEN:
      open.barcode_unknown(kood);
      break;
    case ADMIN:
      admin.barcode_unknown(kood);
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::door_opens() {
  switch (state()) {
    case OPEN:
      open.door_opened();
      break;
    case ADMIN:
      admin.doorOpened();
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::doors_closed() {
  switch (state()) {
    case OPEN:
      open.doors_closed();
      break;
    case ADMIN:
      admin.doorsClosed();
      break;
  }
  return *this;
}

Atm_fridge& Atm_fridge::intrusion_detected() {
  switch (state()) {
    case OPEN:
      // Kas open.l on väärtustatud?
      if (strcmp(open.l.nimi.c_str(), "") != 0) {
        intrusion.intrusion_detected(open.l);
      } else {
        intrusion.intrusion_detected();
      }
      break;
    case CLOSED:
      // kas closed.l on väärtustatud?
      if (strcmp(closed.l.nimi.c_str(), "") != 0) {
        intrusion.intrusion_detected(closed.l);
      } else {
        intrusion.intrusion_detected();
      }
      break;
    case START:
      intrusion.intrusion_detected();
      break;
    case UNKNOWN_NFC:
      intrusion.intrusion_detected();
      break;
    case INTRUSION:
      intrusion.intrusion_detected();
      break;
  }
  trigger(EVT_INTRUSION_DETECTED);
  return *this;
}

Atm_fridge& Atm_fridge::intrusion_over() {
  intrusion.intrusion_over();
  return *this;
}

void Atm_fridge::updateTemp(float temp, float hum) {
  if (isnan(temp)) return;
  char txt[21];
  dtostrf(temp, 6, 1, txt);
  strcat(txt, "°C");
  dtostrf(hum, 9, 1, txt+9);
  strcat(txt, "%");
  draw(2, txt);
}

Atm_fridge& Atm_fridge::exitAdminClosed() {
  sulgeTehinguteks();
  state(START);
  return *this;
}

Atm_fridge& Atm_fridge::exitAdminOpen() {
  avaTehinguteks();
  state(START);
  return *this;
}

Atm_fridge& Atm_fridge::exitIntrusion() {
  state(START);
  return *this;
}

Atm_fridge& Atm_fridge::enterAdmin() {
  // reseti open / closed
  state(ADMIN);
  return *this;
}

Atm_fridge& Atm_fridge::onClearScreen(clear_cb clearScreen) {
  this->clearScreen = clearScreen;
  return *this;
}

Atm_fridge& Atm_fridge::onDraw(draw_cb draw) {
  this->draw = draw;
  return *this;
}

Atm_fridge& Atm_fridge::onDrawF(draw_F_cb draw_F) {
  this->draw_F = draw_F;
  return *this;
}

Atm_fridge& Atm_fridge::onOlemeSuletud(bool_cb olemeSuletud) {
  this->olemeSuletud = olemeSuletud;
  return *this;
}

Atm_fridge& Atm_fridge::onSulgeTehinguteks(sulge_cb sulgeTehinguteks) {
  this->sulgeTehinguteks = sulgeTehinguteks;
  return *this;
}

Atm_fridge& Atm_fridge::onAvaTehinguteks(ava_cb avaTehinguteks) {
  this->avaTehinguteks = avaTehinguteks;
  return *this;
}

Atm_fridge& Atm_fridge::onLock(Machine& machine, int event) {
  onPush(connectors, ON_LOCK, 0, 1, 1, machine, event);
  return *this;
}

Atm_fridge& Atm_fridge::onLock(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_LOCK, 0, 1, 1, callback, idx);
  return *this;
}

Atm_fridge& Atm_fridge::onTempOff(Machine& machine, int event) {
  onPush(connectors, ON_TEMP_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_fridge& Atm_fridge::onTempOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_TEMP_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_fridge& Atm_fridge::onTempOn(Machine& machine, int event) {
  onPush(connectors, ON_TEMP_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_fridge& Atm_fridge::onTempOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_TEMP_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_fridge& Atm_fridge::onClockOn(Machine& machine, int event) {
  onPush(connectors, ON_CLOCK_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_fridge& Atm_fridge::onClockOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_CLOCK_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_fridge& Atm_fridge::onClockOff(Machine& machine, int event) {
  onPush(connectors, ON_CLOCK_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_fridge& Atm_fridge::onClockOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_CLOCK_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_fridge& Atm_fridge::onUnlockChest(Machine& machine, int event) {
  onPush(connectors, ON_UNLOCK_CHEST, 0, 1, 1, machine, event);
  return *this;
}

Atm_fridge& Atm_fridge::onUnlockChest(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_UNLOCK_CHEST, 0, 1, 1, callback, idx);
  return *this;
}

Atm_fridge& Atm_fridge::onUnlockFridge(Machine& machine, int event) {
  onPush(connectors, ON_UNLOCK_FRIDGE, 0, 1, 1, machine, event);
  return *this;
}

Atm_fridge& Atm_fridge::onUnlockFridge(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_UNLOCK_FRIDGE, 0, 1, 1, callback, idx);
  return *this;
}

Atm_fridge& Atm_fridge::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "FRIDGE\0EVT_INTRUSION_DETECTED\0EVT_SD_INSERTED\0EVT_SD_EJECTED\0EVT_MSG_TIMEOUT\0ELSE\0START\0NO_SD\0OPEN\0CLOSED\0ADMIN\0UNKNOWN_NFC\0INTRUSION");
  return *this;
}
