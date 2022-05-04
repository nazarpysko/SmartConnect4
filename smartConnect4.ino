
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

byte rowPins[ROWS] = {22, 24, 26, 28}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {30, 32, 34, 36}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), colPins, rowPins, ROWS, COLS );

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



void userTurn() 
{
  lcdPrintTwoLines("Your turn!", "Enter column:");

  char column = 0;
  while (true) 
  {
    char column = keypad.waitForKey();
    if (validColumn(column)) break;

  lcdPrintTwoLines("Not valid!", "Enter column:");
  }

  lcdPrintTwoLines("Loading...");
  moveColumn(column);
}

void moveColumn(char c) 
{
  int column = int(c); // Esto no va
  Serial.println(column);
  delay(1000);
}

boolean validColumn(char column) 
{
  if (column < 49 || column > 55) return false;

  // TODO: validate if column is not full
  return true;
}

//int readKeyPressed()
//{
// int column = 0;
// while (column == 0) 
// {
//  lcdPrint("Enter column: ");
//  char columnChar = keypad.getKey();   
//  String columnString = "" + columnChar;
//  column = columnString.toInt();
//  if (!isDigit(columnChar) || (column > 0 && column < 8))
//  {
//    lcd.print(columnChar);
//    return column;
//  }
//  lcdPrint("Incorrecto");
// }
// }
