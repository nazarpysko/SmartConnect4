/*
 * @file MyKeypad.h
 * @brief Little extension of Keypad.h to use it in this project
 * 
 */

#ifndef MYKEYPAD_HEADER
#define MYKEYPAD_HEADER

#include <Keypad.h>

#define PAD_COLS 4
#define PAD_ROWS 4


char keys[PAD_ROWS][PAD_COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B' },
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Digital pins to connect the Keypad
byte colPins[PAD_ROWS] = {22, 24, 26, 28}; 
byte rowPins[PAD_COLS] = {30, 32, 34, 36}; 

Keypad keypad = Keypad(makeKeymap(keys), colPins, rowPins, PAD_ROWS, PAD_COLS);

/*
 * @brief Get column from user interaction with the keypad in blocking mode. 
 * @return Column introduced converted from char to byte
 */
byte getColumnFromKeypad() {
  return keypad.waitForKey() - '0';
}

#endif
