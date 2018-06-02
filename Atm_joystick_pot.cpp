#include "Atm_joystick_pot.h"

Atm_joystick_pot& Atm_joystick_pot::begin(Adafruit_ADS1115& ads, byte pin) {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*                     ON_ENTER  ON_LOOP  ON_EXIT  EVT_RELEASE  EVT_HIGH  EVT_LOW  EVT_DEBOUNCE   EVT_DELAY  EVT_REPEAT  ELSE */
    /*       IDLE */             -1,      -1,      -1,          -1,    HWAIT,   LWAIT,           -1,         -1,         -1,   -1,
    /*      HWAIT */             -1,      -1,      -1,        IDLE,       -1,    IDLE,     HPRESSED,         -1,         -1,   -1,
    /*   HPRESSED */   ENT_HPRESSED,      -1,      -1,        IDLE,       -1,    IDLE,           -1, HREPEATING,         -1,   -1,
    /* HREPEATING */   ENT_HPRESSED,      -1,      -1,        IDLE,       -1,    IDLE,           -1,         -1, HREPEATING,   -1,
    /*      LWAIT */             -1,      -1,      -1,        IDLE,     IDLE,      -1,     LPRESSED,         -1,         -1,   -1,
    /*   LPRESSED */   ENT_LPRESSED,      -1,      -1,        IDLE,     IDLE,      -1,           -1, LREPEATING,         -1,   -1,
    /* LREPEATING */   ENT_LPRESSED,      -1,      -1,        IDLE,     IDLE,      -1,           -1,         -1, LREPEATING,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  this->ads = ads;
  this->pin = pin;
  timer_debounce.set(DEBOUNCE);
  timer_delay.set(ATM_TIMER_OFF);
  timer_repeat.set(ATM_TIMER_OFF);
  return *this;          
}

int Atm_joystick_pot::event(int id) {
  int reading;
  switch (id) {
    case EVT_RELEASE:
      reading = ads.readADC_SingleEnded(pin);
      return LOW_THRESHOLD < reading && reading < HIGH_THRESHOLD;
    case EVT_HIGH:
      return ads.readADC_SingleEnded(pin) >= HIGH_THRESHOLD;
    case EVT_LOW:
      return ads.readADC_SingleEnded(pin) <= LOW_THRESHOLD;
    case EVT_DEBOUNCE:
      return timer_debounce.expired(this);
    case EVT_DELAY:
      return timer_delay.expired(this);
    case EVT_REPEAT:
      return timer_repeat.expired(this);
  }
  return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_HIGH, 0, <v>, <up> );
 *   push( connectors, ON_LOW, 0, <v>, <up> );
 */
void Atm_joystick_pot::action(int id) {
  switch (id) {
    case ENT_HPRESSED:
      push(connectors, ON_HIGH, 0, 1, 1);
      return;
    case ENT_LPRESSED:
      push(connectors, ON_LOW, 0, 0, 0);
      return;
  }
}

Atm_joystick_pot& Atm_joystick_pot::debounce(int time) {
  timer_debounce.set(time);
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::repeat(int delay, int speed) {
  timer_delay.set(delay);
  timer_repeat.set(speed);
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::low(int threshold) {
  LOW_THRESHOLD = threshold;
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::high(int threshold) {
  HIGH_THRESHOLD = threshold;
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::onHigh(Machine& machine, int event) {
  onPush(connectors, ON_HIGH, 0, 1, 1, machine, event);
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::onHigh(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_HIGH, 0, 1, 1, callback, idx);
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::onLow(Machine& machine, int event) {
  onPush(connectors, ON_LOW, 0, 1, 1, machine, event);
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::onLow(atm_cb_push_t callback, int idx) {
  onPush(connectors, ON_LOW, 0, 1, 1, callback, idx);
  return *this;
}

Atm_joystick_pot& Atm_joystick_pot::trace(Stream& stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "JOYSTICK_POT\0EVT_RELEASE\0EVT_HIGH\0EVT_LOW\0EVT_DEBOUNCE\0EVT_DELAY\0EVT_REPEAT\0ELSE\0IDLE\0HWAIT\0HPRESSED\0HREPEATING\0LWAIT\0LPRESSED\0LREPEATING");
  return *this;
}

