#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

#define I2C_address 0x3F 
#define PAD_COLS 4
#define PAD_ROWS 4

#define ROWS 6
#define COLS 7

#define USER_TOKEN  'x'
#define AI_TOKEN    'o'

#define EMPTY 10
// Constant to return  if a token is not possible to deploy in a column
#define NOT_VALID 8


// Score constants
// OPEN means there is still chance to make 4 in a row
#define FOUR_IN_A_ROW_SCORE 100
#define OPEN_THREE_IN_A_ROW_SCORE 10
#define OPEN_TWO_IN_A_ROW_SCORE 5
#define OPEN_ODD_THREE_IN_A_ROW_SCORE 80
#define CENTER_SCORE 6

LiquidCrystal_I2C lcd(I2C_address, 16, 2);

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

// The board is labeled from left to right & top to bottom. 
char board[ROWS][COLS] = {
//#COLS:  0    1    2    3    4    5    6    #ROWS
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 0
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 1
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 2
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 3
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 4
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 5
};

// Each column's count of tokens
byte depth[COLS] = {0, 0, 0, 0, 0, 0, 0}; 

typedef struct {
  byte row;
  byte col;
} Position;

typedef struct {
  byte token;
  byte oddToken; 
  byte empty;
} AccountTokens;

void lcdPrint(String s1, String s2="") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
}

void setup() {
  Serial.begin(9600);
  lcd.begin();

  lcd.backlight();
  lcd.cursor();
  lcd.blink();
}

void loop() {
  userTurn();
  checkEndOfGame(USER_TOKEN);
  AITurn();
  checkEndOfGame(AI_TOKEN);
}

void checkEndOfGame(char token) {
  // TODO: Refactor logs. One function with string parameter and token. 
  char buff[50]; 
  snprintf(buff, sizeof(buff),"[%s turn]: Checking for win\n", token == USER_TOKEN ? "User" : "AI");
  Serial.print(buff);
  
  if(isWin(token)) {
    memset(buff, 0, sizeof(buff));
    snprintf(buff, sizeof(buff),"[%s turn]: Won the game. GAME OVER\n", token == USER_TOKEN ? "User" : "AI");
    Serial.println(buff);
    // TODO: custom message depending on who wins
    lcdPrint("GAME OVER", "");
    for(;;);  // Dummy loop to end game
  }

  memset(buff, 0, sizeof(buff));
  snprintf(buff, sizeof(buff),"[%s turn]: Not found connected 4\n", token == USER_TOKEN ? "User" : "AI");
  Serial.print(buff);
}

void AITurn(){
  Serial.println("\n\n[AI turn]: Start");
  
  Position pos = {.row = EMPTY, .col = EMPTY};
  pickBestPosition(AI_TOKEN, &pos);
  
  char buff[50]; 
  snprintf(buff, sizeof(buff),"Row = %d & Column = %d\n", pos.row, pos.col);
  Serial.print(buff);
  
  updateBoard(pos.row, pos.col, AI_TOKEN);
  Serial.println("[AI turn]: End");
}

void countTokens(char *window, char token, AccountTokens *at) {
  for (byte i = 0; i < 4; i++)
    if (window[i] == token) at->token++;
    else if (window[i] == ' ') at->empty++;
    else at->oddToken++;
}

int evaluateWindow(char *window, char token) {
  int score = 0;
  AccountTokens accountTokens = {.token = 0, .oddToken = 0, .empty = 0};
  
  countTokens(window, token, &accountTokens);
  
  if (accountTokens.token == 4) score += FOUR_IN_A_ROW_SCORE;
  else if (accountTokens.token == 3 && accountTokens.empty == 1) score += OPEN_THREE_IN_A_ROW_SCORE;
  else if (accountTokens.token == 2 && accountTokens.empty == 2) score += OPEN_TWO_IN_A_ROW_SCORE;

  if (accountTokens.oddToken == 3 && accountTokens.empty == 1) score -= OPEN_ODD_THREE_IN_A_ROW_SCORE;
  
  return score;
}

int scorePosition(char token) {
  int score = 0;

  // Score center positions
  for (byte row = 0; row < ROWS-3; row++) {
    char window[4] = {board[row][3], board[row+1][3], board[row+2][3], board[row+3][3]};
    AccountTokens at = {0, 0, 0};
    countTokens(window, token, &at);
    score += at.token * CENTER_SCORE;
  }
  
  // Score horizontal positions
  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS - 3; col++) {
      // Create a group of horizontal positions of board to check
      char window[4] = {board[row][col], board[row][col+1], board[row][col+2], board[row][col+3]};
      score += evaluateWindow(window, token);
    }
  }

  // Score vertical positions
  for (byte col = 0; col < COLS; col++) {
    for (byte row = 0; row < ROWS - 3; row++) {
      // Create a group of vertical positions of board to check
      char window[4] = {board[row][col], board[row+1][col], board[row+2][col], board[row+3][col]};
      score += evaluateWindow(window, token);
    }
  }

  // Score positive diagonal
  for (byte row = 0; row < ROWS - 3; row++) {
    for (byte col = 0; col < COLS - 3; col++) {
      // Create a group of positive diagonal positions of board to check
      char window[4] = {board[row][col], board[row+1][col+1], board[row+2][col+2], board[row+3][col+3]};
      score += evaluateWindow(window, token);
    }
  }

  // Score negative diagonal
  for (byte row = 0; row < ROWS - 3; row++) {
    for (byte col = COLS - 4; col < COLS; col++) {
      // Create a group of negative diagonal positions of board to check
      char window[4] = {board[row][col], board[row+1][col-1], board[row+2][col-2], board[row+3][col-3]};
      score += evaluateWindow(window, token);
    }
  }
  
  return score;
}

void pickBestPosition(char token, Position *pos) {
  int bestScore = -INFINITY;

  for (byte col = 0; col < COLS; col++) {
    
    byte row = validateColumn(col); 
    if (row == NOT_VALID) continue;

    board[row][col] = token;
    int score = scorePosition(token);
    
    char buff[50]; 
    snprintf(buff, sizeof(buff),"Trying row = %d & col = %d (score = %d)\n", row, col, score);
    Serial.print(buff);

    printBoard();
    
    board[row][col] = ' ';


    if(pos->col == EMPTY || score > bestScore){
      bestScore = score;
      pos->col = col;
      pos->row = row;
      
      char buff[60]; 
      snprintf(buff, sizeof(buff),"New best score found: %d at row = %d & col = %d\n", bestScore, row, col);
      Serial.print(buff);
    }
  }
}

/**
 * @brief Contains all the logic of user's turn  
 */
void userTurn() {
  Serial.println("\n\n[User turn]: Start");
  lcdPrint("Your turn!", "Enter column:");

  byte column = 0;
  byte row = NOT_VALID;
  while (true) {
    column = keypad.waitForKey() - '0'; 
    
    // OffByOne. From user's perspective columns are numerated from 1 to 7. 
    column--; 
    
    row = validateColumn(column);
    if (row != NOT_VALID) break;
    
    lcdPrint("Not valid!", "Enter column:");
    Serial.println("[User turn]: Not correct column given");
  }

  lcdPrint("Loading...");
  updateBoard(row, column, USER_TOKEN);
  Serial.println("[User turn]: End");
}

/**
 * @brief Print the board on Serial
 */

void printBoard() {
  Serial.println();
  
  Serial.print(' ');
  for (byte col = 0; col < COLS; col++) {
    Serial.print(col);
    Serial.print(' ');
  }

  Serial.println();
  
  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS; col++) {
      char buff[3];
      snprintf(buff, sizeof(buff),"|%c", board[row][col]);
      Serial.print(buff);
    }
    Serial.println("|");
  }
  Serial.println();
}

/**
 * @brief Updates phisical board by moving the mechanism and virtual board state
 * @param c - byte column's number selected 
 */
void updateBoard(byte row, byte column, char token) {
  char buff[40];
  snprintf(buff, sizeof(buff),"[%s turn]: Column selected: %d\n", token == USER_TOKEN ? "User" : "AI", column);
  Serial.print(buff);
  
  board[row][column] = token;
  depth[column] += 1;
  
  printBoard();

  memset(buff, 0, sizeof(buff));
  snprintf(buff, sizeof(buff),"[%s turn]: Moving mechanism...\n", token == USER_TOKEN ? "User" : "AI");
  Serial.print(buff);
  
  // TODO: run mechanism to the deploy the token in selected column 
}

/**
 * @brief It validates if it is possible to push a token in a given column
 * @param column - byte column number selected
 * @return  row value to update the board or NOT_VALID if is not possible
 */
byte validateColumn(byte column) { 
  if (column < 0 || column > 6 || depth[column] == 6) return  NOT_VALID;
  return 5 - depth[column];
}


bool isWin(char token) {
  // TODO: Refactor to avoid checking come positions. Not necessary to check void positions in vertical checkings for example
  
  // Check for horizontals
  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS - 3; col++) {
      if (board[row][col] == token && board[row][col + 1] == token && board[row][col + 2] == token && board[row][col + 3] == token)
        return true;
    }
  }

  // Check for verticals
  for (byte col = 0; col < COLS; col++) {
    for (byte row = 0; row < ROWS - 3; row++) {
      if (board[row][col] == token && board[row + 1][col] == token && board[row + 2][col] == token && board[row + 3][col] == token)
        return true;
    }
  }

  // Check for positive diagonals
  for (byte row = 0; row < ROWS - 3; row++) {
    for (byte col = 0; col < COLS - 3; col++) {
      if (board[row][col] == token && board[row + 1][col + 1] == token && board[row + 2][col + 2] == token && board[row + 3][col + 3] == token)
        return true;
    }
  }
  // Check for negative diagonals
  for (byte row = 0; row < ROWS - 3; row++) {
    for (byte col = COLS - 4; col < COLS; col++) {
      if (board[row][col] == token && board[row + 1][col - 1] == token && board[row + 2][col - 2] == token && board[row + 3][col - 3] == token)
        return true;
    }
  }

  return false;
}
