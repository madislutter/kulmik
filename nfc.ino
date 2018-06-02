#include <Wire.h>
#include <PN532_I2C.h>
#include "PN532.h"

static PN532_I2C pn532i2c(Wire);
static PN532 nfc(pn532i2c);
const int PN532_RSTPDN = D5;
uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
uint8_t uidLen = 0;
String telefoniUID = "";

inline void cycleNFC() {
  if (NFC_ON)
    readNFC();
}

void setupNFC() {
  pinMode(D5, OUTPUT);
  restartPN532();
  nfc.begin();
  Wire.begin(D2, D1);
  Wire.setClockStretchLimit(2000);
  while (!nfc.getFirmwareVersion())
    restartPN532();
  nfc.setPassiveActivationRetries(0x01);
  nfc.SAMConfig();
}

uint32_t getVersion() {
  uint32_t versiondata = nfc.getFirmwareVersion();
  while (!versiondata) {
    Serial.println(F("Didn't find PN53x board"));
    restartPN532();
    versiondata = nfc.getFirmwareVersion();
  }

  return versiondata;
}

void restartPN532() {
  digitalWrite(PN532_RSTPDN, LOW);
  digitalWrite(PN532_RSTPDN, HIGH);
  delay(20);
}

void triggerLiige(String key) {
  Liige l;
  if (findLiigeByUID(key, l)) {
    if (l.saldo > 0 || l.isAdmin) {
      beep();
    } else {
      doubleBeep();
    }
    fridge.nfc(l);
  } else {
    doubleBeep();
    fridge.nfc_unknown(key.c_str());
  }
}

void readNFC() {
  if (viibatakseUutUIDd()) {
    if (UIDonJuhuslikultGenereeritud()) {
      if (loeTelefoniUID()) {
        triggerLiige(telefoniUID);
      } else {
        fridge.nfc_locked();
      }
    } else {
      triggerLiige(kaardiUID());
    }
  }
}

bool validAPDUResponse(uint8_t* response, uint8_t len) {
  return len != 2 || response[0] != 0x6A || response[1] != 0x82;
}

bool viibatakseUutUIDd() {
  uint8_t newUID[] = {0, 0, 0, 0, 0, 0, 0};
  uint8_t newUIDlen;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &newUID[0], &newUIDlen)) {
    if (memcmp(uid, newUID, newUIDlen) != 0) { // Uus kaart
      memset(uid, 0, uidLen);
      memcpy(uid, newUID, newUIDlen);
      uidLen = newUIDlen;
      return true;
    } else { // Lugesime sama kaarti kaks korda järjest.
      return false;
    }
  } else { // Kaarti ei viibata praegu
    memset(uid, 0, uidLen);
    return false;
  }
}

bool UIDonJuhuslikultGenereeritud() {
  // 0x08-ga algavad UID'd on juhuslikult genereeritud. Osad telefonid annavad UID 01:02:03:04. Ka seda me ei arvesta, vaid üritame sel puhul äpiga suhelda.
  return uid[0] == 0x08 || (uid[0] == 0x01 && uid[1] == 0x02 && uid[2] == 0x03 && uid[3] == 0x04 && uidLen == 4);
}

bool loeTelefoniUID() {
  nfc.inRelease();
  if (nfc.inListPassiveTarget()) {
//                          CLA,  INS,  P1,   P2,   AID length, AID
    uint8_t selectAPDU[] = {0x00, 0xA4, 0x04, 0x00, 0x05,       0xF0, 0x18, 0x84, 0x18, 0x84};
    uint8_t response[32];
    uint8_t responseLen = 32;
    if (nfc.inDataExchange(selectAPDU, sizeof(selectAPDU), response, &responseLen)) {
      if (validAPDUResponse(response, responseLen)) {
        telefoniUID = extractUIDFromAPDU(response, responseLen);
        return true;
      } else {
        //Serial << "Jama APDU vastus" << endl;
      }
    } else {
      //Serial << "APDU fail" << endl;
    }
  } else {
    //Serial << "inList fail" << endl;
  }
  return false;
}

String kaardiUID() {
  char buf[uidLen*3];
  char* pointer = &buf[0];
  for (byte i = 0; i < uidLen; i++) {
    if (i>0) pointer += sprintf(pointer, ":");
    pointer += sprintf(pointer, "%02X", uid[i]);
  }
  return buf;
}

String extractUIDFromAPDU(uint8_t* response, uint8_t responseLen) {
  String result = "";
  for (int i=0; i < responseLen; i++) {
    char c = response[i];
    if (c > 0x1f && c <= 0x7f)
      result += c;
  }
  return result;
}

