#include "Atm_admin.h"

const String menuItems = 
"-Ava uksed:101"
"-Ost sularahaga:102"
//"-Ürituse olekusse:103"
"-Sulge tehinguteks:104"
"-Restart:105";
Menu menuu(menuItems);

Atm_admin& Atm_admin::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                                      ON_ENTER  ON_LOOP         ON_EXIT    EVT_MENUTIMEOUT  EVT_MSGTIMEOUT   EVT_DOOR_OPENED   EVT_DOORS_CLOSED  ELSE */
    /*               MENU */                ENT_MENU, LP_MENU,             -1,              EXIT,             -1,               -1,                -1,   -1,
    /*         HOIA_LAHTI */          ENT_HOIA_LAHTI,      -1, EXT_HOIA_LAHTI,                -1,           MENU,               -1,              MENU,   -1,
    /*       SULARAHA_OST */        ENT_SULARAHA_OST,      -1,             -1,                -1,           MENU,    TAKE_PRODUCTS,                -1,   -1,
    /*      TAKE_PRODUCTS */       ENT_TAKE_PRODUCTS,      -1,             -1,                -1,             -1,               -1,     SCAN_PRODUCTS,   -1,
    /*      SCAN_PRODUCTS */       ENT_SCAN_PRODUCTS,      -1,             -1,              EXIT,             -1,               -1,                -1,   -1,
    /*               DEAL */                ENT_DEAL,      -1,             -1,                -1,           MENU,               -1,                -1,   -1,
    /*    UNKNOWN_PRODUCT */     ENT_UNKNOWN_PRODUCT,      -1,             -1,                -1,           MENU,               -1,                -1,   -1,
    /*               EXIT */                ENT_EXIT,      -1,             -1,                -1,             -1,               -1,                -1,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  timer_menu_timeout.set(60000); // 1min
  timer_msg_timeout.set(10000); // 10s
  menuu.defineLcd(20, 4);
  menuu.mapKeys(UP, DOWN, LEFT, RIGHT);
  this->l = {};
  return *this;          
}

int Atm_admin::event(int id) {
  switch (id) {
    case EVT_MENUTIMEOUT:
      return timer_menu_timeout.expired(this);
    case EVT_MSGTIMEOUT:
      return timer_msg_timeout.expired(this);
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push(connectors, ON_EXIT_CLOSED, 0, <v>, <up>);
 *   push(connectors, ON_EXIT_OPEN, 0, <v>, <up>);
 *   push(connectors, ON_LOCK, 0, <v>, <up>);
 *   push(connectors, ON_UNLOCK, 0, <v>, <up>);
 */

void Atm_admin::action(int id) {
  char txt[21];
  switch (id) {
    case ENT_MENU:
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_NFC_OFF, 0, 0, 0);
      this->t = {};
      menuu.done();
      return;
    case LP_MENU:
      showMenu();
      kontrolliTegevust();
      return;
    case ENT_HOIA_LAHTI:
      push(connectors, ON_UNLOCK, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Hoian uksi lahti"));
      return;
    case EXT_HOIA_LAHTI:
      push(connectors, ON_LOCK, 0, 0, 0);
      return;
    case ENT_SULARAHA_OST:
      push(connectors, ON_UNLOCK, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Sularahatehing"));
      draw_F(1, F("Skänni toode!"));
      return;
    case ENT_TAKE_PRODUCTS:
      clearScreen();
      draw_F(0, F("Sularahatehing"));
      draw_F(1, F("Võta toode!"));
      return;
    case ENT_SCAN_PRODUCTS:
      push(connectors, ON_LOCK, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Sularahatehing"));
      draw_F(1, F("Skänni toode!"));
      return;
    case ENT_DEAL:
      push(connectors, ON_LOCK, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Tehing sooritatud"));
      draw(1, this->t.nimi.c_str());

      txt[0] = '\0';
      strcat(txt, "Hind: ");
      dtostrf(this->t.hind / 100.0, 6, 2, txt + 6);
      strcat(txt, "€");
      draw(2, txt);

      salvestaTehing(this->l, this->t);
      draw_F(3, F("Skänni veel kui vaja"));
      return;
    case ENT_UNKNOWN_PRODUCT:
      push(connectors, ON_LOCK, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Tundmatu toode!"));
      draw_F(2, F("Tehingut ei"));
      draw_F(3, F("salvestatud."));
      return;
    case ENT_EXIT:
      push(connectors, ON_EXIT_OPEN, 0, 0, 0);
      return;
  }
}

void Atm_admin::showMenu() {
  if (menuu.needsUpdate()) {
    clearScreen();
    for (int i = 0; i < 4; i++)
      draw(i, menuu.lcdLine(i).c_str());
    menuu.updated();
  }
}

void Atm_admin::kontrolliTegevust() {
  if (menuTegevus > 0) {
    switch (menuTegevus) {
      case 101:
        state(HOIA_LAHTI);
        break;
      case 102:
        state(SULARAHA_OST);
        break;
      case 103:
        push(connectors, ON_EXIT_YRITUS, 0, 0, 0);
        break;
      case 104:
        push(connectors, ON_EXIT_CLOSED, 0, 0, 0);
        break;
      case 105:
        clearScreen();
        draw_F(0, F("Restardin..."));
        ESP.restart();
        break;
    }
    menuu.done();
    menuTegevus = 0;
  }
}

void Atm_admin::up() {
  this->state_millis = millis();
  switch (state()) {
    case MENU:
      menuTegevus = menuu.updateWith(UP);
      return;
  }
}

void Atm_admin::down() {
  this->state_millis = millis();
  switch (state()) {
    case MENU:
      menuTegevus = menuu.updateWith(DOWN);
      return;
  }
}

void Atm_admin::left() {
  this->state_millis = millis();
  switch (state()) {
    case MENU:
      push(connectors, ON_EXIT_OPEN, 0, 0, 0);
      return;
    case TAKE_PRODUCTS:
      return;
    default:
      state(MENU);
  }
}

void Atm_admin::right() {
  this->state_millis = millis();
  switch (state()) {
    case MENU:
      menuTegevus = menuu.updateWith(RIGHT);
      return;
  }
}

void Atm_admin::reset() {
  state(MENU);
}

void Atm_admin::doorOpened() {
  trigger(EVT_DOOR_OPENED);
}

void Atm_admin::doorsClosed() {
  trigger(EVT_DOORS_CLOSED);
}

void Atm_admin::barcode(Toode t) {
  switch (state()) {
    case SULARAHA_OST:
    case SCAN_PRODUCTS:
    case DEAL:
      this->t = {t.nimi, t.hind, t.barcode, t.kaal};
      state(DEAL);
      return;
  }
}

void Atm_admin::barcode_unknown(char* code) {
  switch (state()) {
    case SULARAHA_OST:
    case SCAN_PRODUCTS:
    case DEAL:
      //this->t = {code, 0, code, 0};
      state(UNKNOWN_PRODUCT);
      return;
  }
}

Atm_admin& Atm_admin::onClearScreen(clear_cb clearScreen) {
  this->clearScreen = clearScreen;
  return *this;
}

Atm_admin& Atm_admin::onDraw(draw_cb draw) {
  this->draw = draw;
  return *this;
}

Atm_admin& Atm_admin::onDrawF(draw_F_cb draw_F) {
  this->draw_F = draw_F;
  return *this;
}

Atm_admin& Atm_admin::onSalvestaTehing(salvesta_tehing_cb salvestaTehing) {
  this->salvestaTehing = salvestaTehing;
  return *this;
}

Atm_admin& Atm_admin::onExitClosed(Machine& machine, int event) {
  onPush(connectors, ON_EXIT_CLOSED, 0, 1, 1, machine, event);
  return *this;
}

Atm_admin& Atm_admin::onExitClosed(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_EXIT_CLOSED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_admin& Atm_admin::onExitOpen(Machine& machine, int event) {
  onPush(connectors, ON_EXIT_OPEN, 0, 1, 1, machine, event);
  return *this;
}

Atm_admin& Atm_admin::onExitOpen(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_EXIT_OPEN, 0, 1, 1, callback, idx);
  return *this;
}

Atm_admin& Atm_admin::onExitYritus(Machine& machine, int event) {
  onPush(connectors, ON_EXIT_YRITUS, 0, 1, 1, machine, event);
  return *this;
}

Atm_admin& Atm_admin::onExitYritus(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_EXIT_YRITUS, 0, 1, 1, callback, idx);
  return *this;
}

Atm_admin& Atm_admin::onLock(Machine& machine, int event) {
  onPush(connectors, ON_LOCK, 0, 1, 1, machine, event);
  return *this;
}

Atm_admin& Atm_admin::onLock(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_LOCK, 0, 1, 1, callback, idx);
  return *this;
}

Atm_admin& Atm_admin::onUnlock(Machine& machine, int event) {
  onPush(connectors, ON_UNLOCK, 0, 1, 1, machine, event);
  return *this;
}

Atm_admin& Atm_admin::onUnlock(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_UNLOCK, 0, 1, 1, callback, idx);
  return *this;
}

Atm_admin& Atm_admin::onNFCoff(Machine& machine, int event) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_admin& Atm_admin::onNFCoff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_admin& Atm_admin::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "ADMIN\0EVT_MENUTIMEOUT\0EVT_MSGTIMEOUT\0EVT_DOOR_OPENED\0EVT_DOORS_CLOSED\0ELSE\0MENU\0HOIA_LAHTI\0SULARAHA_OST\0TAKE_PRODUCTS\0SCAN_PRODUCTS\0DEAL\0UNKNOWN_PRODUCT\0EXIT");
  return *this;
}
