const int MCP23017_ADDRESS = 0x20;
const int MCP23017_IODIRA = 0x00;
const int MCP23017_PULLUPA = 0x0C;
const int MCP23017_PORTA = 0x12;

const uint8_t SIREN_PIN = 0;
const uint8_t LOCK_PIN = 1;
const uint8_t PIEZO_PIN = 2;
const uint8_t JOYSTICK_BUTTON_PIN = 3;
const uint8_t DOOR_PIN = 4;

byte MCP23017_value = 0x00;

void setupMCP23017() {
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write(MCP23017_IODIRA);
  Wire.write(1 << JOYSTICK_BUTTON_PIN | 1 << DOOR_PIN); // A3 ja A4 INPUT
  Wire.endTransmission();

  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write(MCP23017_PULLUPA);
  Wire.write(1 << JOYSTICK_BUTTON_PIN | 1 << DOOR_PIN); // A3 ja A4 INPUT_PULLUP
  Wire.endTransmission();

  writeMCP23017A(0x00);
}

void sirenOn() {
  writeMCP23017A(MCP23017_value | 1 << SIREN_PIN);
}

void sirenOff() {
  writeMCP23017A(MCP23017_value & ~(1 << SIREN_PIN));
}

void unlockFridge() {
  writeMCP23017A(MCP23017_value | 1 << LOCK_PIN);
}

void lockFridge() {
  writeMCP23017A(MCP23017_value & ~(1 << LOCK_PIN));
}

void piezoOn() {
  writeMCP23017A(MCP23017_value | 1 << PIEZO_PIN);
}

void piezoOff() {
  writeMCP23017A(MCP23017_value & ~(1 << PIEZO_PIN));
}

void piezoToggle() {
  writeMCP23017A(MCP23017_value ^ 1 << PIEZO_PIN);
}

bool doorOpen() {
  return MCP23017_value & 1 << DOOR_PIN;
}

bool joystickBtnPressed() {
  return !(MCP23017_value & 1 << JOYSTICK_BUTTON_PIN);
}

byte readMCP23017A() {
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write(MCP23017_PORTA);
  Wire.endTransmission();
  Wire.requestFrom(MCP23017_ADDRESS, 1);
  MCP23017_value = Wire.read();
  return MCP23017_value;
}

void writeMCP23017A(byte value) {
  MCP23017_value = value;
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write(MCP23017_PORTA);
  Wire.write(value);
  Wire.endTransmission();
}
