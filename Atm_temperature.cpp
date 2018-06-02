#include "Atm_temperature.h"
#include "DHT.h"

DHT lugeja(D6, DHT21);

Atm_temperature& Atm_temperature::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*               ON_ENTER    ON_LOOP  ON_EXIT   EVT_ON  EVT_OFF  EVT_DELAY  ELSE */
    /*   SLEEP */          -1, ATM_SLEEP,      -1, READING,      -1,        -1,   -1,
    /* READING */ ENT_READING,        -1,      -1,      -1,   SLEEP,   READING,   -1,
  };
  // clang-format on
  Machine::begin(state_table, ELSE);
  timer_delay.set(10000);
  return *this;
}

int Atm_temperature::event(int id) {
  switch (id) {
    case EVT_DELAY:
      return timer_delay.expired(this);
  }
  return 0;
}

void Atm_temperature::action(int id) {
  switch (id) {
    case ENT_READING:
      update(lugeja.readTemperature(), lugeja.readHumidity());
      return;
  }
}

Atm_temperature& Atm_temperature::on() {
  trigger(EVT_ON);
  return *this;
}

Atm_temperature& Atm_temperature::off() {
  trigger(EVT_OFF);
  return *this;
}

Atm_temperature& Atm_temperature::setInterval(unsigned long interval) {
  timer_delay.set(interval);
  return *this;
}

Atm_temperature& Atm_temperature::onUpdate(update_cb update) {
  this->update = update;
  return *this;
}

Atm_temperature& Atm_temperature::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "TEMPERATURE\0EVT_ON\0EVT_OFF\0EVT_DELAY\0ELSE\0SLEEP\0READING");
  return *this;
}
