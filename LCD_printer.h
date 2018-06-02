#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class LCD_printer: public Print {
  public:
    LCD_printer& begin(uint8_t DATA, uint8_t CLK, uint8_t address, byte cols, byte rows);
    LCD_printer& printRow(byte row, const char* txt);
    LCD_printer& printRow(byte row, const __FlashStringHelper* txt);
    LCD_printer& printCenteredRow(byte row, char* txt);
    LCD_printer& printCenteredRow(byte row, const __FlashStringHelper* txt);
    LCD_printer& printMultiline(char* txt);
    LCD_printer& printMultiline(const __FlashStringHelper* fString);
    LCD_printer& scrollDown();
    LCD_printer& scrollUp();
    LCD_printer& clear();
    virtual size_t write(uint8_t);
  private:
    LiquidCrystal_I2C lcd;
    byte screenCols, screenRows;
    byte scrollPosition, rowsInBuffer;
    static const byte BUFFER_LEN = 240; // Määrab, kui pikka teksti saab printMultiline funktsiooniga kirjutada.
    char BUFFER[BUFFER_LEN] = {0};
    byte sha[8] = { 10, 4, 14, 16, 14, 1, 30 }; // š
    byte euro[8] = { 14, 17, 28, 16, 28, 17, 14 }; // €
    byte soo[8] = { 10, 20, 14, 17, 17, 17, 14 }; // õ
    void printMultiline(byte startingFromRow);
    byte print(char* txt);
    byte leftPaddingForCentering(char* txt);
    int textwidth(char* txt);
    byte charsInRow(char* txt);
    byte countRowsInBuffer();
};

