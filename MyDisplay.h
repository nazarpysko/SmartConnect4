/*
 * @file MyDisplay.h
 * @brief LCD display initialization and little function for displaying messages easily
 * 
 */

#ifndef MYDISPLAY_HEADER
#define MYDISPLAY_HEADER

#include <LiquidCrystal_I2C.h>

#define DISPLAY_WIDTH   16
#define DISPLAY_HEIGHT  2

#define I2C_address 0x3F // May need to be changed in your board. See README for more details.

LiquidCrystal_I2C lcd(I2C_address, DISPLAY_WIDTH, DISPLAY_HEIGHT);


/*
 * @brief Setup funtion. Used it in setup() in the main file. 
 */
void setupDisplay() {
  lcd.begin();
  lcd.backlight();
  lcd.cursor();
  lcd.blink();
}


/*
 * @brief Print two strings in the LCD display. The strings length should be lower than 17 characters
 */
void lcdPrint(String s1, String s2="") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
}

#endif
