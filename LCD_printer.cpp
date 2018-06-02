#include "LCD_printer.h"

LCD_printer& LCD_printer::begin(uint8_t DATA, uint8_t CLK, uint8_t address, byte cols, byte rows) {
  lcd.begin(DATA, CLK, address, cols, rows);
  lcd.createChar(0, sha);
  lcd.createChar(1, euro);
  lcd.createChar(2, soo);
  lcd.home();
  this->screenCols = cols;
  this->screenRows = rows;
  this->scrollPosition = 0;
  this->rowsInBuffer = 0;
}

LCD_printer& LCD_printer::printRow(byte row, const char* txt) {
  this->rowsInBuffer = 0; // Kerimise keelamiseks
  lcd.setCursor(0, row);

  byte charsPrinted = 0, pointer = 0;
  while (charsPrinted < screenCols && txt[pointer] != '\0')
    charsPrinted += write(txt[pointer++]);

  while (charsPrinted < screenCols) {
    lcd.write(' ');
    charsPrinted++;
  }
}

LCD_printer& LCD_printer::printRow(byte row, const __FlashStringHelper* fString) {
  char txt[screenCols+10]; // Kuna täpitähed võtavad kaks kohta, siis teeme puhvri igaks juhuks pikema
  strncpy_P(txt, (const char *)fString, screenCols+9);
  printRow(row, txt);
}

LCD_printer& LCD_printer::printCenteredRow(byte row, char* txt) {
  this->rowsInBuffer = 0; // Kerimise keelamiseks
  lcd.setCursor(0, row);
  byte charsPrinted = 0;

  byte leftPadding = leftPaddingForCentering(txt);
  while (charsPrinted < leftPadding) {
    lcd.write(' ');
    charsPrinted++;
  }

  byte pointer = 0;
  while (charsPrinted < screenCols && txt[pointer] != '\0')
    charsPrinted += write(txt[pointer++]);

  while (charsPrinted < screenCols) {
    lcd.write(' ');
    charsPrinted++;
  }
}

LCD_printer& LCD_printer::printCenteredRow(byte row, const __FlashStringHelper* fString) {
  char txt[screenCols+10]; // Kuna täpitähed võtavad kaks kohta, siis teeme puhvri igaks juhuks pikema
  strncpy_P(txt, (const char *)fString, screenCols+9);
  printCenteredRow(row, txt);
}

LCD_printer& LCD_printer::printMultiline(char* txt) {
  scrollPosition = 0;
  strncpy(BUFFER, txt, BUFFER_LEN-1);
  rowsInBuffer = countRowsInBuffer();
  printMultiline(scrollPosition);
}

LCD_printer& LCD_printer::printMultiline(const __FlashStringHelper* fString) {
  scrollPosition = 0;
  strncpy_P(BUFFER, (const char *)fString, 240);
  rowsInBuffer = countRowsInBuffer();
  printMultiline(scrollPosition);
}

LCD_printer& LCD_printer::scrollDown() {
  if (scrollPosition + screenRows < rowsInBuffer)
    printMultiline(++scrollPosition);
}

LCD_printer& LCD_printer::scrollUp() {
  if (scrollPosition > 0)
    printMultiline(--scrollPosition);
}

LCD_printer& LCD_printer::clear() {
  lcd.clear();
  return *this;
}

/**
 * @return amount of characters written on screen.
 * 
 * Return value helps caller keep track how many characters fit on the screen after printing.
 */
inline size_t LCD_printer::write(uint8_t c) {
  if (195 == (int)c || 226 == (int)c || 172 == (int)c || 197 == (int)c || 176 == (int)c)
    return 0; // skip special char modifiers
  if (181 == (int)c || 149 == (int)c) {
    lcd.write(byte(2)); // õ
  } else if (164 == (int)c || 132 == (int)c) {
    lcd.print((char) - 31); // ä
  } else if (182 == (int)c || 150 == (int)c) {
    lcd.print((char) - 17); // ö
  } else if (188 == (int)c || 156 == (int)c) {
    lcd.print((char) - 11); // ü
  } else if (161 == (int)c || 160 == (int)c) {
    lcd.write(byte(0)); // š
  } else if (130 == (int)c) {
    lcd.write(byte(1)); // €
  } else if (194 == (int)c) {
    lcd.write(char(223)); // °
  } else {
    lcd.write(c); // not a special character
  }
  return 1;
}

void LCD_printer::printMultiline(byte startingFromRow) {
  lcd.clear();
  int textLen = strlen(BUFFER);

  byte pointer = 0;
  // Keri startingFromRow rea võrra pointerit edasi.
  while (startingFromRow-- > 0 && pointer < textLen)
    pointer += charsInRow(&BUFFER[pointer]);

  byte row = 0;
  while (row < screenRows && pointer < textLen) {
    lcd.setCursor(0, row++);
    pointer += print(&BUFFER[pointer]);
  }
}

/**
 * @return amount of chars looped through.
 * 
 * Return value helps caller keep track where we are in the buffer after printing.
 */
inline byte LCD_printer::print(char* txt) {
  byte pointer = 0, charsPrinted = 0;
  while (charsPrinted < screenCols && txt[pointer] != '\0')
    charsPrinted += write(txt[pointer++]);
  return pointer;
}

inline byte LCD_printer::leftPaddingForCentering(char* txt) {
  int width = textwidth(txt);
  if (width < screenCols)
    return floor((screenCols - width) / 2);
  return 0;
}

inline int LCD_printer::textwidth(char* txt) {
  int c, i = 0, chars = 0;
  while (txt[i] != '\0') {
    c = txt[i++];
    if (195 != c && 226 != c && 172 != c && 197 != c && 176 != c)
      chars++;
  }

  return chars;
}

inline byte LCD_printer::charsInRow(char* txt) {
  byte amount = 0, pointer = 0;
  int c;
  while (amount < screenCols && txt[pointer] != '\0') {
    c = txt[pointer++];
    if (195 != c && 226 != c && 172 != c && 197 != c && 176 != c)
      amount++;
  }

  return pointer;
}

byte inline LCD_printer::countRowsInBuffer() {
  byte row = 0;
  int textLen = strlen(BUFFER);
  byte pointer = 0;
  while (pointer < textLen) {
    pointer += charsInRow(&BUFFER[pointer]);
    row++;
  }

  return row;
}

