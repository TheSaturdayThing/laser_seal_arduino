/*****************************************
 * name: I2C LCD2004
 * function: You should now see your I2C LCD2004 display "Hello,world!","IIC/I2C LCD2004"
 * "20 cols, 4 rows","www.sunfounder.com"
 ********************************/
//Email:service@sunfounder.com
//Website:www.sunfounder.com

/********************************/
// Include necessary libraries
#include "LCD.h"
#include "Midi_db.h"

#define CHARS 20
#define LINES 4

// Initialize the LCD object, set the LCD I2C address to 0x27 for a 20x4 display
LiquidCrystal_I2C lcd(0x27, 20, 4);  

/*********************************************************/
// Initialize LCD
void initLCD() {
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("  Welcome to 4-409  ");

  lcd.setCursor(0, 1);
  lcd.print("Turn knob to select");

  // Clear lines 3 and 4 initially
  lcd.setCursor(0, 2);
  lcd.print("                    "); 
  lcd.setCursor(0, 3);
  lcd.print("                    "); 
}

// Helper: print a string truncated/padded to 20 chars
void lcdPrintLine(int row, const String &text) {
  String out = text;
  if (out.length() > 20) {
    out = out.substring(0, 20);       // truncate
  } else {
    while (out.length() < 20) out += " "; // pad with spaces
  }
  lcd.setCursor(0, row);
  lcd.print(out);
}

// Function to display MIDI DB info
void displayMidi(int number) {
  Midi m = getMidi(number);

  // Line 3: index : category
  String line3 = String(m.number) + ":" + String(m.category);
  lcdPrintLine(2, line3);

  // Line 4: instrument
  String line4 = String(m.instrument);
  lcdPrintLine(3, line4);
}


