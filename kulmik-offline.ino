#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <Wire.h>
#include "FS.h"
#include "Abiheader.h"
#include <Automaton.h>
#include "Atm_joystick_pot.h"
#include "Atm_barcode.h"
#include "Atm_clock.h"
#include "Atm_btn.h"
#include "LCD_printer.h"

#include "Atm_fridge.h"
#include "Atm_open.h"
#include "Atm_closed.h"
#include "Atm_admin.h"
#include "Atm_intrusion.h"
#include "Atm_lock.h"
#include "Atm_door.h"
#include "Atm_temperature.h"

const char* liikmedFail = "/liikmed";
const char* tootedFail = "/tooted";
const char* tehingudFail = "/tehingud";
const char* logiFail = "/logi";
const char* seadedFail = "/seaded";

const char* SSID = "";
const char* PASSWORD = "";

Atm_open open;
Atm_closed closed;
Atm_admin admin;
Atm_intrusion intrusion;
Atm_fridge fridge;
Atm_temperature temperature;
Atm_lock fridgeLocks;
Atm_door doors;
Atm_clock clock;
Atm_timer piezoBeeper;
Atm_timer piezoNBeeper; // beeps N times
Adafruit_ADS1115 ads;
Atm_joystick_pot horizontal;
Atm_joystick_pot vertical;
Atm_btn btn;
Atm_barcode barcode;
const int BARCODE_BUFFER_LEN = 45;
char BARCODE_BUFFER[BARCODE_BUFFER_LEN];
bool NFC_ON = false;
LCD_printer lcd;

void setup() {
  Serial.begin(115200);
  Serial << endl << F("Main") << endl;

  lcd.begin(D2, D1, 0x3F, 20, 4);
  delay(50); // LCD vajab inittimiseks aega vist.
  lcd.clear().printCenteredRow(0, F("Käivitun"));

  SPIFFS.begin();

//  SPIFFS.open(tehingudFail, "w").close();
//  String txt = readAll(f);
//  f.close();
//  Serial << "tehingudFail:" << endl << txt << endl;
  
  setupJoystick();
  setupMCP23017();
  barcode.begin(D3, D4, BARCODE_BUFFER, BARCODE_BUFFER_LEN).onScanned([] (int idx, int v, int up) {
    Toode t;
    if (findToodeByBAR(BARCODE_BUFFER, t)) {
      fridge.barcode(t);
    } else {
      fridge.barcode_unknown(BARCODE_BUFFER);
    }
  });//.trace(Serial);
  temperature.begin().onUpdate([] (float temp, float hum) { fridge.updateTemp(temp, hum); });
  clock.begin().onTick([] () { printTime(); if (timeToRestart()) { ESP.restart(); } });
  piezoBeeper.begin(500).repeat(-1).onTimer([] (int idx, int v, int up) { piezoToggle(); });
  piezoNBeeper.begin(200).repeat(6).onTimer([] (int idx, int v, int up) { piezoToggle(); });
  setupFridge();
  setupDoors();
  setupLocks();
  lcd.printRow(1, F("Automaadid ok"));

  setupNFC();
  lcd.printRow(2, F("NFC ok"));

  // Kui uks on lahti, siis palume ukse sulgeda ja enne ei jätka
  readMCP23017A();
  if (doorOpen()) {
    lcd.clear().printMultiline(F("Uks on lahti. Palun sulge uks!"));
    while (doorOpen()) {
      yield();
      readMCP23017A();
    }
    lcd.clear().printCenteredRow(0, F("Käivitun"));
    lcd.printRow(1, F("Automaadid ok"));
    lcd.printRow(2, F("NFC ok"));
  }

  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SSID, PASSWORD);
  }
  Serial << F("WiFi connecting...") << endl;
  lcd.printRow(3, F("Ühendun netti..."));
  while (WiFi.status() != WL_CONNECTED) {
    Serial << F(".");
    delay(500);
  }
  Serial << F("WiFi connected") << endl;
  
  lcd.clear().printRow(0, F("NTP"));
  setupNTP();
  yield();
  
  lcd.printRow(1, F("Pärin liikmed"));
  if (uuendaLiikmed()) {
    lcd.printRow(1, F("Pärin liikmed +"));
    // Lahuta saldodest mälus olevate tehingute summad
    korrigeeriSaldod();
    lcd.printRow(1, F("Pärin liikmed ++"));
  } else {
    Serial << F("Liikmete päring ebaõnnestus") << endl;
    lcd.printRow(1, F("Pärin liikmed -"));
  }
  yield();
  
  lcd.printRow(2, F("Pärin tooted"));
  if (uuendaTooted()) {
    lcd.printRow(2, F("Pärin tooted +"));
  } else {
    Serial << F("Toodete päring ebaõnnestus!") << endl;
    lcd.printRow(2, F("Pärin tooted -"));
  }
  yield();

  
  lcd.printRow(3, F("Salvestan tehingud"));
  if (salvestaSalvestamataTehingud()) {
    lcd.printRow(3, F("Salvestan tehingud +"));
  } else {
    Serial << F("Salvestamata tehingute salvestamine ebaõnnestus") << endl;
    lcd.printRow(3, F("Salvestan tehingud -"));
  }

  Serial << F("Setup done") << endl;
}

void loop() {
  unsigned long t0 = millis();
  cycleNFC();
  unsigned long t1 = millis();
  if (t1-t0 > 3000) {
    Serial << F("NFC jooksis kokku. Teen uuesti setupNFC().") << endl;
    setupNFC();
  }
  yield();
  barcode.cycle();
  yield();
  temperature.cycle();
  yield();
  clock.cycle();
  piezoBeeper.cycle();
  piezoNBeeper.cycle();
  yield();
  readMCP23017A();
  if (doorOpen()) {
    doors.opened();
  } else {
    doors.closed();
  }
  if (joystickBtnPressed()) {
    btn.pressed();
  } else {
    btn.released();
  }
  yield();
  horizontal.cycle();
  vertical.cycle();
  btn.cycle();
  yield();
  doors.cycle();
  fridgeLocks.cycle();
  yield();
  fridge.cycle();
}

void setupJoystick() {
  ads.begin();
  vertical.begin(ads, 0).low(120).high(15000) // centered at 8695
    .onHigh( [](int idx, int v, int up) { fridge.right(); })
    .onLow( [](int idx, int v, int up) { fridge.left(); });
  horizontal.begin(ads, 1).low(120).high(15000).repeat(500, 100)  // centered at 8662
    .onHigh( [](int idx, int v, int up) { fridge.up(); })
    .onLow( [](int idx, int v, int up) { fridge.down(); });
  btn.begin().onPressed([] (int idx, int v, int up) { fridge.right(); });
}

void setupDoors() {
  doors.begin()//.trace(Serial)
    .onOpened([] (int idx, int v, int up) { fridgeLocks.opened(); fridge.door_opens(); })
    .onClosed([] (int idx, int v, int up) { fridgeLocks.closed(); fridge.doors_closed(); });
}

void setupLocks() {
  fridgeLocks.begin().lockingDelay(10000)//.trace(Serial) // Lukustab ukse 10s pärast luku avamist, kui ust ei avata
    .onLocked([] (int idx, int v, int up) { lockFridge(); })
    .onUnlocked([] (int idx, int v, int up) { unlockFridge(); })
    .onIntrusionDetected([] (int idx, int v, int up) { fridge.intrusion_detected(); })
    .onIntrusionOver([] (int idx, int v, int up) { fridge.intrusion_over(); });
}

void setupFridge() {
  open//.trace(Serial)
    .onClearScreen([] () { lcd.clear(); })
    .onDraw([] (byte row, const char* buffer) { lcd.printRow(row, buffer); })
    .onDrawF([] (byte row, const __FlashStringHelper* buffer) { lcd.printRow(row, buffer); })
    .onSalvestaTehing([] (Liige &l, Toode &t) { yield(); return salvestaTehing(l, t); })
    .onExitToAdmin([] (int idx, int v, int up) { fridge.enterAdmin(); })
    .onNFCon([] (int idx, int v, int up) { NFC_ON = true; })
    .onNFCoff([] (int idx, int v, int up) { NFC_ON = false; })
    .onBarcodeOn(barcode, barcode.EVT_ON)
    .onBarcodeOff(barcode, barcode.EVT_OFF)
    .onTempOn(temperature, temperature.EVT_ON)
    .onTempOff(temperature, temperature.EVT_OFF)
    .onClockOn(clock, clock.EVT_ON)
    .onClockOff(clock, clock.EVT_OFF)
    .onLock([] (int idx, int v, int up) { fridgeLocks.lock(); /*chestLocks.lock();*/ })
    .onUnlockFridge([] (int idx, int v, int up) { fridgeLocks.unlock(); })
    .onBeepingOn([] (int idx, int v, int up) { piezoBeeper.start(); })
    .onBeepingOff([] (int idx, int v, int up) { piezoBeeper.stop(); piezoOff(); })
    .onBeepThreeTimes([] () { piezoNBeeper.start(); });
  closed//.trace(Serial)
    .onDraw([] (byte row, const char* buffer) { lcd.printRow(row, buffer); })
    .onDrawF([] (byte row, const __FlashStringHelper* buffer) { lcd.printRow(row, buffer); })
    .onClearScreen([] () { lcd.clear(); })
    .onExitToAdmin([] (int idx, int v, int up) { fridge.enterAdmin(); })
    .onTempOn(temperature, temperature.EVT_ON)
    .onTempOff(temperature, temperature.EVT_OFF)
    .onClockOn(clock, clock.EVT_ON)
    .onClockOff(clock, clock.EVT_OFF)
    .onNFCon([] (int idx, int v, int up) { NFC_ON = true; })
    .onNFCoff([] (int idx, int v, int up) { NFC_ON = false; });
  admin//.trace(Serial)
    .onClearScreen([] () { lcd.clear(); })
    .onDraw([] (byte row, const char* buffer) { lcd.printRow(row, buffer); })
    .onDrawF([] (byte row, const __FlashStringHelper* buffer) { lcd.printRow(row, buffer); })
    .onExitClosed([] (int idx, int v, int up) { fridge.exitAdminClosed(); })
    .onExitOpen([] (int idx, int v, int up) { fridge.exitAdminOpen(); })
    .onLock([] (int idx, int v, int up) { fridgeLocks.lock(); })
    .onUnlock([] (int idx, int v, int up) { fridgeLocks.unlock(); })
    .onNFCoff([] (int idx, int v, int up) { NFC_ON = false; })
    .onSalvestaTehing([] (Liige &l, Toode &t) { yield(); return salvestaTehing(l, t); });
  intrusion//.trace(Serial)
    .onClearScreen([] () { lcd.clear(); })
    .onDraw([] (byte row, const char* content) { lcd.printRow(row, content); })
    .onDrawF([] (byte row, const __FlashStringHelper* buffer) { lcd.printRow(row, buffer); })
    .onExit([] (int idx, int v, int up) { fridge.exitIntrusion(); })
    .onSirenOff([] (int idx, int v, int up) { sirenOff(); })
    .onSirenOn([] (int idx, int v, int up) { sirenOn(); /*vargusAlgas();*/ })
    .onNFCon([] (int idx, int v, int up) { NFC_ON = true; })
    .onNFCoff([] (int idx, int v, int up) { NFC_ON = false; })
    .onSalvestaSissetungija([] (Liige &l) {
      /*yield();
      if (strcmp(l.nimi.c_str(), "") == 0) {
        return vargusVaigistatud(String("uid,") + l.nfc_id);
      } else {
        return vargusVaigistatud(l.nimi);
      }*/ return true;
    });
  fridge//.trace(Serial)
    .onClearScreen([] () { lcd.clear(); })
    .onDraw([] (byte row, const char* buffer) { lcd.printRow(row, buffer); })
    .onDrawF([] (byte row, const __FlashStringHelper* buffer) { lcd.printRow(row, buffer); })
    .onLock([] (int idx, int v, int up) { fridgeLocks.lock(); })
    .onUnlockFridge([] (int idx, int v, int up) { fridgeLocks.unlock(); })
    .onUnlockChest([] (int idx, int v, int up) { /*chestLocks.unlock();*/ })
    .onTempOff(temperature, temperature.EVT_OFF)
    .onTempOn(temperature, temperature.EVT_ON)
    .onClockOn(clock, clock.EVT_ON)
    .onClockOff(clock, clock.EVT_OFF)
    .onOlemeSuletud([] () { return olemeSuletud(); })
    .onSulgeTehinguteks([] () { sulgeTehinguteks(); })
    .onAvaTehinguteks([] () { avaTehinguteks(); })
    .begin(open, closed, admin, intrusion)
    .cycle();
}

inline void beep() {
  piezoOn();
  delay(50);
  piezoOff();
}

inline void doubleBeep() {
  beep();
  delay(50);
  beep();
}
