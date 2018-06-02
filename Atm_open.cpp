#include "Atm_open.h"

Atm_open& Atm_open::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                                           ON_ENTER  ON_LOOP                ON_EXIT  EVT_DOOR_OPENED  EVT_DOORS_CLOSED        EVT_TIMEOUT    EVT_WAITFORSCAN  ELSE */
    /*                  IDLE */                  ENT_IDLE,      -1,              EXT_IDLE,              -1,               -1,                -1,                -1,   -1,
    /*          SALDO_LOCKED */          ENT_SALDO_LOCKED,      -1,                    -1,              -1,               -1,          ENT_IDLE,                -1,   -1,
    /*        SALDO_UNLOCKED */        ENT_SALDO_UNLOCKED,      -1,                    -1,    TAKE_PRODUCT,               -1,          ENT_IDLE,                -1,   -1,
    /*          TAKE_PRODUCT */          ENT_TAKE_PRODUCT,      -1,                    -1,              -1,     SCAN_PRODUCT,                -1,                -1,   -1,
    /*          SCAN_PRODUCT */          ENT_SCAN_PRODUCT,      -1,                    -1,    TAKE_PRODUCT,               -1,                -1, SCAN_PRODUCT_LONG,   -1,
    /*     SCAN_PRODUCT_LONG */     ENT_SCAN_PRODUCT_LONG,      -1, EXT_SCAN_PRODUCT_LONG,    TAKE_PRODUCT,               -1,                -1,                -1,   -1,
    /*                  DEAL */                  ENT_DEAL,      -1,                    -1,              -1,               -1,          ENT_IDLE,                -1,   -1,
    /*               NO_DEAL */               ENT_NO_DEAL,      -1,                    -1,              -1,               -1,          ENT_IDLE,                -1,   -1,
    /*          NO_MORE_DEAL */          ENT_NO_MORE_DEAL,      -1,                    -1,  RETURN_PRODUCT,               -1,          ENT_IDLE,                -1,   -1,
    /*        RETURN_PRODUCT */        ENT_RETURN_PRODUCT,      -1,                    -1,              -1,             IDLE,                -1,                -1,   -1,
    /*       UNKNOWN_PRODUCT */       ENT_UNKNOWN_PRODUCT,      -1,                    -1,              -1,               -1,          ENT_IDLE,                -1,   -1,
    /* UNKNOWN_PRODUCT_RETRY */ ENT_UNKNOWN_PRODUCT_RETRY,      -1,                    -1,              -1,               -1,                -1, SCAN_PRODUCT_LONG,   -1,
    /*                  EXIT */                  ENT_EXIT,      -1,                    -1,              -1,               -1,                -1,                -1,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  timer_timeout.set(10000); // 10s
  timer_waitForScan.set(30000); // 30s
  return *this;
}

int Atm_open::event(int id) {
  switch (id) {
    case EVT_TIMEOUT:
      return timer_timeout.expired(this);
    case EVT_WAITFORSCAN:
      return timer_waitForScan.expired(this);
  }
  return 0;
}

/* Add C++ code for each action
   This generates the 'output' for the state machine

   Available connectors:
     push(connectors, ON_BARCODE_OFF, 0, <v>, <up>);
     push(connectors, ON_BARCODE_ON, 0, <v>, <up>);
     push(connectors, ON_EXIT_ADMIN, 0, <v>, <up>);
     push(connectors, ON_LOCK, 0, <v>, <up>);
     push(connectors, ON_NFC_OFF, 0, <v>, <up>);
     push(connectors, ON_NFC_ON, 0, <v>, <up>);
     push(connectors, ON_TEMP_OFF, 0, <v>, <up>);
     push(connectors, ON_TEMP_ON, 0, <v>, <up>);
     push(connectors, ON_CLOCK_ON, 0, <v>, <up>);
     push(connectors, ON_CLOCK_OFF, 0, <v>, <up>);
     push(connectors, ON_BEEPING_ON, 0, <v>, <up>);
     push(connectors, ON_BEEPING_OFF, 0, <v>, <up>);
*/

void Atm_open::action(int id) {
  float saldo;
  char txt[21];
  txt[0] = '\0';
  switch (id) {
    case ENT_IDLE:
      nrOfWrongBarcodesScanned = 0;
      push(connectors, ON_LOCK, 0, 0, 0);
      this->l = {};
      this->t = {};
      clearScreen();
      draw_F(0, F("Ostmiseks viipa NFCd"));
      push(connectors, ON_TEMP_ON, 0, 0, 0);
      push(connectors, ON_CLOCK_ON, 0, 0, 0);
      push(connectors, ON_NFC_ON, 0, 0, 0);
      push(connectors, ON_BARCODE_OFF, 0, 0, 0);
      push(connectors, ON_BEEPING_OFF, 0, 0, 0);
      return;
    case EXT_IDLE:
      push(connectors, ON_TEMP_OFF, 0, 0, 0);
      push(connectors, ON_CLOCK_OFF, 0, 0, 0);
      return;
    case ENT_SALDO_LOCKED:
      push(connectors, ON_LOCK, 0, 0, 0);
      clearScreen();
      strcat(txt, "Tere, ");
      strncat(txt, this->l.nimi.c_str(), 14);
      draw(0, txt);
      
      txt[0] = '\0';
      saldo = this->l.saldo / 100.0;
      strcat(txt, "Saldo: ");
      dtostrf(saldo, 6, 2, txt + 7);
      strcat(txt, "€");
      draw(1, txt);
      if (this->l.saldo < 0) {
        draw_F(2, F("Mis vahid, maksa"));
        draw_F(3, F("võlg ära!"));
      } else if (this->l.saldo == 0) {
        draw_F(2, F("Ostmiseks vajad ette"));
        draw_F(3, F("maksu!"));
      }
      return;
    case ENT_SALDO_UNLOCKED:
      push(connectors, ON_UNLOCK_FRIDGE, 0, 0, 0);
      push(connectors, ON_BARCODE_ON, 0, 0, 0);
      this->t = {};
      clearScreen();
      strcat(txt, "Tere, ");
      strncat(txt, this->l.nimi.c_str(), 14);
      draw(0, txt);
      
      txt[0] = '\0';
      saldo = this->l.saldo / 100.0;
      strcat(txt, "Saldo: ");
      dtostrf(this->l.saldo / 100.0, 6, 2, txt + 7);
      strcat(txt, "€");
      draw(1, txt);

      return;
    case ENT_TAKE_PRODUCT:
      push(connectors, ON_NFC_OFF, 0, 0, 0);
      push(connectors, ON_BARCODE_OFF, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Võta tooted!"));
      return;
    case ENT_SCAN_PRODUCT:
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_BARCODE_ON, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Skänni kõik tooted!"));
      draw_F(1, F("Kui sa midagi ei võt"));
      draw_F(2, F("nud, liiguta juhthoo"));
      draw_F(3, F("ba vasakule."));
      return;
    case ENT_SCAN_PRODUCT_LONG:
      push(connectors, ON_BEEPING_ON, 0, 0, 0);
      draw_F(0, F("Skänni tooted triip-"));
      draw_F(1, F("koodi lugejaga!"));
      draw_F(2, F("Tühistamiseks liigut"));
      draw_F(3, F("a juhthooba vasakule"));
      return;
    case EXT_SCAN_PRODUCT_LONG:
      push(connectors, ON_BEEPING_OFF, 0, 0, 0);
      return;
    case ENT_DEAL:
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_NFC_OFF, 0, 0, 0);
      clearScreen();
      draw(0, this->t.nimi.c_str());

      txt[0] = '\0';
      strcat(txt, "Hind: ");
      dtostrf(this->t.hind / 100.0, 13, 2, txt + 6);
      strcat(txt, "€");
      draw(1, txt);

      txt[0] = '\0';
      strcat(txt, "Uus saldo: ");
      dtostrf((this->l.saldo - this->t.hind) / 100.0, 8, 2, txt + 11);
      strcat(txt, "€");
      draw(2, txt);

      salvestaTehing(this->l, this->t);
      draw_F(3, F("Skänni kõik tooted!"));
      return;
    case ENT_NO_MORE_DEAL:
      push(connectors, ON_UNLOCK_FRIDGE, 0, 0, 0);
      push(connectors, ON_BARCODE_OFF, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Sul pole piisavalt"));
      draw_F(1, F("ettemaksu. Tehingut"));
      draw_F(2, F("ei salvestatud!"));
      draw_F(3, F("Pane toode tagasi!"));
      beepThreeTimes();
      return;
    case ENT_RETURN_PRODUCT:
      clearScreen();
      draw_F(0, F("Pane toode heaga"));
      draw_F(1, F("tagasi!"));
      return;
    case ENT_NO_DEAL:
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_NFC_ON, 0, 0, 0);
      push(connectors, ON_BARCODE_OFF, 0, 0, 0);
      clearScreen();
      draw_F(0, F("Midagi ei ostetud!"));
      return;
    case ENT_UNKNOWN_PRODUCT:
      push(connectors, ON_LOCK, 0, 0, 0);
      push(connectors, ON_NFC_ON, 0, 0, 0);
      push(connectors, ON_BARCODE_OFF, 0, 0, 0);
      draw_F(0, F("Tundmatu triipkood."));
      draw_F(1, F("Palu joogivanemal se"));
      draw_F(2, F("lle toote triipkood "));
      draw_F(3, F("registreerida."));
      return;
    case ENT_UNKNOWN_PRODUCT_RETRY:
      push(connectors, ON_LOCK, 0, 0, 0);
      nrOfWrongBarcodesScanned++;
      clearScreen();
      draw_F(0, F("Tundmatu triipkood:"));
      draw(1, this->t.barcode.c_str());
      if (nrOfWrongBarcodesScanned == 1) {
        draw_F(3, F("Skänni uuesti!"));
      } else {
        draw_F(3, F("Skänni korra veel!"));
      }
      beepThreeTimes();
      return;
    case ENT_EXIT:
      push(connectors, ON_EXIT_ADMIN, 0, 0, 0);
      return;
  }
}

void Atm_open::nfc(Liige l) {
  // Kui sama liige skannib oma kaarti kaks korda järjest ja on admin
  if ((state() == SALDO_LOCKED || state() == SALDO_UNLOCKED) && strcmp(l.nimi.c_str(), this->l.nimi.c_str()) == 0 && l.isAdmin) {
    // Mine admin menüüsse
    state(EXIT);
    return;
  }
  this->l = {l.nimi, l.saldo, l.nfc_id, l.isAdmin};
  if (l.saldo > 0 || l.isAdmin) {
    state(SALDO_UNLOCKED);
  } else {
    state(SALDO_LOCKED);
  }
}

void Atm_open::barcode(Toode t) {
  this->t = {t.nimi, t.hind, t.barcode, t.kaal};
  switch (state()) {
    case SCAN_PRODUCT:
    case SCAN_PRODUCT_LONG:
    case UNKNOWN_PRODUCT_RETRY:
    case SALDO_UNLOCKED:
      state(DEAL);
      break;
    case DEAL:
      if (this->l.saldo > 0 || this->l.isAdmin) {
        state(DEAL);
      } else {
        state(NO_MORE_DEAL);
      }
      break;
  }
}

void Atm_open::barcode_unknown(char* kood) {
  switch (state()) {
    case SCAN_PRODUCT:
    case SCAN_PRODUCT_LONG:
    case UNKNOWN_PRODUCT_RETRY:
    case SALDO_UNLOCKED:
      this->t = {kood, 0, kood, 0};
      if (nrOfWrongBarcodesScanned < 2) {
        state(UNKNOWN_PRODUCT_RETRY);
      } else {
        nrOfWrongBarcodesScanned = 0;
        salvestaTehing(this->l, this->t);
        state(UNKNOWN_PRODUCT);
      }
      break;
    case DEAL:
      if (this->l.saldo > 0 || this->l.isAdmin) {
        this->t = {kood, 0, kood, 0};
        if (nrOfWrongBarcodesScanned < 2) {
          state(UNKNOWN_PRODUCT_RETRY);
        } else {
          nrOfWrongBarcodesScanned = 0;
          salvestaTehing(this->l, this->t);
          state(UNKNOWN_PRODUCT);
        }
      } else {
        state(NO_MORE_DEAL);
      }
      break;
  }
}

void Atm_open::up() {}

void Atm_open::down() {}

void Atm_open::left() {
  switch (state()) {
    case SCAN_PRODUCT:
    case SCAN_PRODUCT_LONG:
      state(NO_DEAL);
      break;
    case SALDO_LOCKED:
    case SALDO_UNLOCKED:
    case DEAL:
    case NO_DEAL:
    case NO_MORE_DEAL:
    case UNKNOWN_PRODUCT:
    case UNKNOWN_PRODUCT_RETRY:
      state(IDLE);
      break;
  }
}

void Atm_open::right() {}

void Atm_open::door_opened() {
  trigger(EVT_DOOR_OPENED);
}

void Atm_open::doors_closed() {
  trigger(EVT_DOORS_CLOSED);
}

void Atm_open::reset() {
  state(IDLE);
}

Atm_open& Atm_open::onBeepThreeTimes(clear_cb beepThreeTimes) {
  this->beepThreeTimes = beepThreeTimes;
  return *this;
}

Atm_open& Atm_open::onClearScreen(clear_cb clearScreen) {
  this->clearScreen = clearScreen;
  return *this;
}

Atm_open& Atm_open::onDraw(draw_cb draw) {
  this->draw = draw;
  return *this;
}

Atm_open& Atm_open::onDrawF(draw_F_cb draw_F) {
  this->draw_F = draw_F;
  return *this;
}

Atm_open& Atm_open::onSalvestaTehing(salvesta_tehing_cb salvestaTehing) {
  this->salvestaTehing = salvestaTehing;
  return *this;
}

Atm_open& Atm_open::onBarcodeOff(Machine& machine, int event) {
  onPush(connectors, ON_BARCODE_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onBarcodeOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_BARCODE_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onBarcodeOn(Machine& machine, int event) {
  onPush(connectors, ON_BARCODE_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onBarcodeOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_BARCODE_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onExitToAdmin(Machine& machine, int event) {
  onPush(connectors, ON_EXIT_ADMIN, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onExitToAdmin(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_EXIT_ADMIN, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onLock(Machine& machine, int event) {
  onPush(connectors, ON_LOCK, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onLock(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_LOCK, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onNFCoff(Machine& machine, int event) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onNFCoff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_NFC_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onNFCon(Machine& machine, int event) {
  onPush(connectors, ON_NFC_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onNFCon(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_NFC_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onTempOff(Machine& machine, int event) {
  onPush(connectors, ON_TEMP_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onTempOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_TEMP_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onTempOn(Machine& machine, int event) {
  onPush(connectors, ON_TEMP_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onTempOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_TEMP_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onClockOn(Machine& machine, int event) {
  onPush(connectors, ON_CLOCK_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onClockOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_CLOCK_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onClockOff(Machine& machine, int event) {
  onPush(connectors, ON_CLOCK_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onClockOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_CLOCK_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onUnlockFridge(Machine& machine, int event) {
  onPush(connectors, ON_UNLOCK_FRIDGE, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onUnlockFridge(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_UNLOCK_FRIDGE, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onBeepingOn(Machine& machine, int event) {
  onPush(connectors, ON_BEEPING_ON, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onBeepingOn(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_BEEPING_ON, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::onBeepingOff(Machine& machine, int event) {
  onPush(connectors, ON_BEEPING_OFF, 0, 1, 1, machine, event);
  return *this;
}

Atm_open& Atm_open::onBeepingOff(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_BEEPING_OFF, 0, 1, 1, callback, idx);
  return *this;
}

Atm_open& Atm_open::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
                    "OPEN\0EVT_DOOR_OPENED\0EVT_DOORS_CLOSED\0EVT_TIMEOUT\0EVT_WAITFORSCAN\0ELSE\0IDLE\0SALDO_LOCKED\0SALDO_UNLOCKED\0TAKE_PRODUCT\0SCAN_PRODUCT\0SCAN_PRODUCT_LONG\0DEAL\0NO_DEAL\0NO_MORE_DEAL\0RETURN_PRODUCT\0UNKNOWN_PRODUCT\0UNKNOWN_PRODUCT_RETRY\0EXIT");
  return *this;
}
