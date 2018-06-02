#pragma once

#include <Automaton.h>
#include <Adafruit_ADS1015.h>

class Atm_joystick_pot: public Machine {

 public:
  enum { IDLE, HWAIT, HPRESSED, HREPEATING, LWAIT, LPRESSED, LREPEATING }; // STATES
  enum { EVT_RELEASE, EVT_HIGH, EVT_LOW, EVT_DEBOUNCE, EVT_DELAY, EVT_REPEAT, ELSE }; // EVENTS
  Atm_joystick_pot(void) : Machine() {};
  Atm_joystick_pot& begin(Adafruit_ADS1115& ads, byte pin);
  Atm_joystick_pot& trace(Stream& stream);
  Atm_joystick_pot& onHigh(Machine& machine, int event = 0);
  Atm_joystick_pot& onHigh(atm_cb_push_t callback, int idx = 0);
  Atm_joystick_pot& onLow(Machine& machine, int event = 0);
  Atm_joystick_pot& onLow(atm_cb_push_t callback, int idx = 0);
  Atm_joystick_pot& debounce(int time);
  Atm_joystick_pot& repeat(int delay, int speed);
  Atm_joystick_pot& low(int threshold);
  Atm_joystick_pot& high(int threshold);

 private:
  enum { ENT_HPRESSED, ENT_LPRESSED }; // ACTIONS
  enum { ON_HIGH, ON_LOW, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  atm_timer_millis timer_debounce, timer_delay, timer_repeat;
  static const int DEBOUNCE = 15;
  int LOW_THRESHOLD = 10;
  int HIGH_THRESHOLD = 1012;
  Adafruit_ADS1115 ads;
  byte pin;
  int event(int id);
  void action(int id);
};

