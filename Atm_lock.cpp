#include "Atm_lock.h"

Atm_lock& Atm_lock::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                               ON_ENTER  ON_LOOP    ON_EXIT       EVT_UNLOCK        EVT_LOCK     EVT_OPENED       EVT_CLOSED  EVT_DELAY    ELSE */
    /*          LOCKED */          ENT_LOCKED,      -1,        -1, UNLOCKED_CLOSED,             -1,         ALARM,              -1,        -1,     -1,
    /*   UNLOCKED_OPEN */   ENT_UNLOCKED_OPEN,      -1,        -1,              -1,             -1,            -1,          LOCKED,        -1,     -1,
    /* UNLOCKED_CLOSED */ ENT_UNLOCKED_CLOSED,      -1,        -1, UNLOCKED_CLOSED,         LOCKED, UNLOCKED_OPEN,              -1,    LOCKED,     -1,
    /*           ALARM */           ENT_ALARM,      -1, EXT_ALARM,   UNLOCKED_OPEN,             -1,            -1,          LOCKED,        -1,     -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  locking_delay.set(10000); // 10s
  return *this;
}

int Atm_lock::event(int id) {
  switch (id) {
    case EVT_DELAY:
      return locking_delay.expired(this);
  }
  return 0;
}

void Atm_lock::action(int id) {
  switch (id) {
    case ENT_LOCKED:
      push(connectors, ON_LOCKED, 0, 1, 1);
      return;
    case ENT_UNLOCKED_OPEN:
      push(connectors, ON_UNLOCKED, 0, 0, 0);
      return;
    case ENT_UNLOCKED_CLOSED:
      push(connectors, ON_UNLOCKED, 0, 0, 0);
      return;
    case ENT_ALARM:
      push(connectors, ON_INTRUSION_DETECTED, 0, 0, 0);
      return;
    case EXT_ALARM:
      push(connectors, ON_INTRUSION_OVER, 0, 0, 0);
      return;
  }
}

Atm_lock& Atm_lock::lockingDelay(int delay) {
  locking_delay.set(delay);
  return *this;
}

void Atm_lock::lock() {
  trigger(EVT_LOCK);
}

void Atm_lock::unlock() {
  trigger(EVT_UNLOCK);
}

void Atm_lock::opened() {
  trigger(EVT_OPENED);
}

void Atm_lock::closed() {
  trigger(EVT_CLOSED);
}

Atm_lock& Atm_lock::onIntrusionDetected(Machine& machine, int event) {
  onPush(connectors, ON_INTRUSION_DETECTED, 0, 1, 1, machine, event);
  return *this;
}

Atm_lock& Atm_lock::onIntrusionDetected(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_INTRUSION_DETECTED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_lock& Atm_lock::onIntrusionOver(Machine& machine, int event) {
  onPush(connectors, ON_INTRUSION_OVER, 0, 1, 1, machine, event);
  return *this;
}

Atm_lock& Atm_lock::onIntrusionOver(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_INTRUSION_OVER, 0, 1, 1, callback, idx);
  return *this;
}

Atm_lock& Atm_lock::onLocked(Machine& machine, int event) {
  onPush(connectors, ON_LOCKED, 0, 1, 1, machine, event);
  return *this;
}

Atm_lock& Atm_lock::onLocked(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_LOCKED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_lock& Atm_lock::onUnlocked(Machine& machine, int event) {
  onPush(connectors, ON_UNLOCKED, 0, 1, 1, machine, event);
  return *this;
}

Atm_lock& Atm_lock::onUnlocked(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_UNLOCKED, 0, 1, 1, callback, idx);
  return *this;
}

Atm_lock& Atm_lock::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "LOCK\0EVT_UNLOCK\0EVT_LOCK\0EVT_OPENED\0EVT_CLOSED\0EVT_DELAY\0ELSE\0LOCKED\0UNLOCKED_OPEN\0UNLOCKED_CLOSED\0ALARM");
  return *this;
}
