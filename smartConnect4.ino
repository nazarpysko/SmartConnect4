
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define I2C_address 0x3F 
#define COLS 4
#define ROWS 4

LiquidCrystal_I2C lcd(I2C_address, 16, 2);

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B' },
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Digital pins to connect the Keypad
byte colPins[ROWS] = {22, 24, 26, 28}; 
byte rowPins[COLS] = {30, 32, 34, 36}; 

Keypad keypad = Keypad(makeKeymap(keys), colPins, rowPins, ROWS, COLS);

void lcdPrintTwoLines(String s1, String s2="") 
{
  lcd.clear();
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
}

void lcdPrintOneLine(String str) 
{
 lcd.clear();
 lcd.setCursor(0,0);
 if (str.length() > 16)
 {
  int spaceIndex = str.indexOf(' ', 0);
  String line0;
  String line1;
    
  if (spaceIndex == -1) 
  {
    spaceIndex = 15; // When there is not space index, the string is divided in two halfs
  }
  else
  {
    spaceIndex = str.lastIndexOf(' ', 16);
  }
  line0 = str.substring(0, spaceIndex + 1);
  line1 = str.substring(spaceIndex + 1);

  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
  
  return ;
 }
 
 lcd.print(str); 
}

void setup()
{
  Serial.begin(9600);
  lcd.begin();

  lcd.backlight();
  lcd.cursor();
  lcd.blink();
  lcdPrintTwoLines("Vaya cara de pringao jaja xD");
}

void loop()
{
  userTurn();
//  checkWinState();  
//  AITurn();
//  checkWinState();
}


/**
 * @brief Contains all the logic of user's turn 
 */
void userTurn() 
{
  Serial.println("[User turn]: Start");
  lcdPrintTwoLines("Your turn!", "Enter column:");

  int column = 0;
  while (true) 
  {
    column = keypad.waitForKey() - '0'; 
    if (validColumn(column)) break;
    
    lcdPrintTwoLines("Not valid!", "Enter column:");
    Serial.println("[User turn]: Not correct column given");
  }

  lcdPrintTwoLines("Loading...");
  moveColumn(column);
  Serial.println("[User turn]: End");
}

/**
 * @brief Moves the mechanism to push the token in a valid column
 * @param c - int column's number selected 
 */
void moveColumn(int column) 
{
  char buffer[50];
  sprintf(buffer, "[User turn]: Column entered: %d\n", column);
  Serial.print(buffer);
  Serial.println("[User turn]: moving mechanism...");
  delay(1000);
  // TODO: run stepmotor to the column & remove above delay
}

/**
 * @brief It validates if it is possible to push a token in a given column
 * @param column - char column number selected
 */
boolean validColumn(int column) 
{
  if (column < 1 || column > 7) return false;

  // TODO: validate if column is not full
  return true;
}
