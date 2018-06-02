#include "Atm_clock.h"

Atm_clock& Atm_clock::begin() {
  // clang-format off
  const static state_t state_table[] PROGMEM = {
    /*          ON_ENTER  ON_LOOP  ON_EXIT  EVT_ON  EVT_OFF  EVT_TICK  ELSE */
    /*   OFF */       -1,      -1,      -1,     ON,      -1,       -1,   -1,
    /*    ON */   ENT_ON,      -1,      -1,     -1,     OFF,       ON,   -1,
  };
  // clang-format on
  Machine::begin( state_table, ELSE );
  timer_tick.set(1000);

  return *this;
}

int Atm_clock::event(int id) {
  switch (id) {
    case EVT_TICK:
      return timer_tick.expired(this);
  }
  return 0;
}

void Atm_clock::action( int id ) {
  switch ( id ) {
    case ENT_ON:
      tick();
      return;
  }
}

Atm_clock& Atm_clock::on() {
  trigger(EVT_ON);
  return *this;
}

Atm_clock& Atm_clock::off() {
  trigger(EVT_OFF);
  return *this;
}

Atm_clock& Atm_clock::onTick(tick_cb tick) {
  this->tick = tick;
  return *this;
}

Atm_clock& Atm_clock::trace(Stream & stream) {
  Machine::setTrace(&stream, atm_serial_debug::trace,
    "CLOCK\0EVT_ON\0EVT_OFF\0EVT_TICK\0ELSE\0OFF\0ON");
  return *this;
}
