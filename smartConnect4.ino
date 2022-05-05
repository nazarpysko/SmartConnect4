
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define I2C_address 0x3F 
#define COLS 4
#define ROWS 4

#define USER_TOKEN  'x'
#define AI_TOKEN    'o'

// Constant to return if a token is not possible to deploy in a column
#define NOT_VALID 8

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

// The board is labeled from left to right & top to bottom. 
char board[6][7] = {
//#Cols:  0    1    2    3    4    5    6   #Rows
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 0
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 1
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 2
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 3
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 4
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 5
};

// Each column's count of tokens
byte depth[7] = {0, 0, 0, 0, 0, 0, 0};

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

  byte column = 0;
  byte row = NOT_VALID;
  while (true) 
  {
    column = keypad.waitForKey() - '0'; 
    
    // OffByOne. From user's perspective columns are numerated from 1 to 7. 
    column--; 
    
    row = validateColumn(column);
    if (row != NOT_VALID) break;
    
    lcdPrintTwoLines("Not valid!", "Enter column:");
    Serial.println("[User turn]: Not correct column given");
  }

  lcdPrintTwoLines("Loading...");
  updateBoard(row, column, USER_TOKEN);
  Serial.println("[User turn]: End");
}

/**
 * @brief Print the board on Serial
 */

void printBoard()
{
  Serial.println();
  for (byte row = 0; row < 6; row++)
  {
    for (byte col = 0; col < 7; col++) 
    {
      char buff[2];
      sprintf(buff, "|%c", board[row][col]);
      Serial.print(buff);
    }  
    Serial.println("|");
  }
  Serial.println();
}

/**
 * @brief Updates phisical board by moving the mechanism and virtual board state
 * @param c - int column's number selected 
 */
void updateBoard(byte row, byte column, byte turnToken) 
{
  char buffer[50];
  sprintf(buffer, "[%s turn]: Column selected: %\n", turnToken == USER_TOKEN ? "User" : "AI", column + 1);
  Serial.print(buffer);
  
  board[row][column] = turnToken;
  depth[column] += 1;
  
  printBoard();

  sprintf(buffer, "[%s turn]: Moving mechanism...\n", turnToken == USER_TOKEN ? "User" : "AI", column + 1);
  Serial.print(buffer);
  
  // TODO: run mechanism to the deploy the token in selected column 
}

/**
 * @brief It validates if it is possible to push a token in a given column
 * @param column - byte column number selected
 * @return row value to update the board or NOT_VALID if is not possible
 */
byte validateColumn(byte column) 
{ 
  if (column < 0 || column > 6 || depth[column] == 6) return NOT_VALID;

  return 5 - depth[column];
}
