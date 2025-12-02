#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal_I2C.h>

// Expose the LCD object so other files can use it
extern LiquidCrystal_I2C lcd;

// Function prototypes
void initLCD();
void displayMidi(int number);

#endif
