#include <Wire.h> 
#include "DeploySystem.h"
#include "MyDisplay.h"
#include "MyKeypad.h"

#define TEST_SERVO
//#define DEBUG
#define MINIMAX // Comment if you don't want to play with minimax algorithm

#define ROWS 6
#define COLS 7

#define USER_TOKEN  'x'
#define AI_TOKEN    'o'

#define EMPTY 10
#define NOT_VALID 8


// Score constants
// OPEN means there is still chance to make 4 in a row
#define FOUR_IN_A_ROW_SCORE 100
#define OPEN_THREE_IN_A_ROW_SCORE 5
#define OPEN_TWO_IN_A_ROW_SCORE 2
#define OPEN_ODD_THREE_IN_A_ROW_SCORE 80
#define CENTER_SCORE 3

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

typedef struct {
  byte row;
  byte col;
} Position;

typedef struct {
  int score; 
  Position position;
} MinimaxResult;

typedef struct {
  byte token;
  byte oddToken; 
  byte empty;
} AccountTokens;

void setup() {
  // TERMINAL setup
  Serial.begin(9600);

  // 2x16 LCD DISPLAY setup
  setupDisplay();

  // SERVO setup
  setupServo();

  // LIMIT SWITCH setup
  setupLimitSwitch();
  
  // STEP MOTOR setup
  setupStepMotor();
  
  // TEST
  char currentTurn = USER_TOKEN;
  referenceSelector();
  
  for (int col = 0; col < 7; col++) {
    deployToken(currentTurn, col);
    currentTurn = currentTurn == USER_TOKEN ? AI_TOKEN : USER_TOKEN;
  }
}

void loop() {
  #ifndef TEST_SERVO
  userTurn();
  checkEndOfGame(USER_TOKEN);
  AITurn();
  checkEndOfGame(AI_TOKEN);
  #endif
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
    
    lcdPrint("GAME OVER", token == USER_TOKEN ? "You win!!" : "You lose :(");
    for(;;);  // Dummy loop to end game
  }

  memset(buff, 0, sizeof(buff));
  snprintf(buff, sizeof(buff),"[%s turn]: Not found connected 4\n", token == USER_TOKEN ? "User" : "AI");
  Serial.print(buff);
}

void AITurn(){
  Serial.println("\n\n[AI turn]: Start");

  #ifndef MINIMAX
  Position pos = {.row = EMPTY, .col = EMPTY};
  pickBestPosition(AI_TOKEN, &pos);

  char buff[50]; 
  snprintf(buff, sizeof(buff),"Row = %d & Column = %d\n", pos.row, pos.col);
  Serial.print(buff);

  updateBoard(pos.row, pos.col, AI_TOKEN);
  Serial.println("[AI turn]: End");
  #endif

  #ifdef MINIMAX
  MinimaxResult mmr = minimax(3 , -INFINITY, INFINITY, true);
  
  char buff[50]; 
  snprintf(buff, sizeof(buff),"[AI turn]: Row = %d & Column = %d\n", mmr.position.row, mmr.position.col);
  Serial.print(buff);
  
  updateBoard(mmr.position.row, mmr.position.col, AI_TOKEN);
  Serial.println("[AI turn]: End");
  #endif
}

void countTokens(char *window, char token, AccountTokens *at) {
  for (byte i = 0; i < 4; i++)
    if (window[i] == token) at->token++;
    else if (window[i] == ' ') at->empty++;
    else at->oddToken++;
}

int evaluateWindow(char *window, char token) {
  int score = 0;
  AccountTokens accountTokens = {.token=0, .oddToken=0, .empty=0};
  
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

boolean isTerminalNode(void) {
  bool boardFull = false;
  for (byte col = 0; col < COLS; col++) {
    // If not empty spot found, board is not empty 
    if (board[0][col] == ' ') break;  
    
    // If all columns checked and still here, board is full
    if (col == COLS - 1) boardFull = true; 
  }
  
  return boardFull || isWin(USER_TOKEN) || isWin(AI_TOKEN);
}

MinimaxResult minimax(byte depth, int alpha, int beta, bool maximizingPlayer) {
  #ifdef DEBUG
  Serial.print("===== MINIMAX =====\n");
  #endif
  
  MinimaxResult minimaxResult;
  minimaxResult.position = {NOT_VALID, NOT_VALID};
  
  #ifdef DEBUG
  char buff[50]; 
  snprintf(buff, sizeof(buff),"----- Trying depth = %d -----\n", depth);
  Serial.print(buff);
  
  printBoard();
  #endif
  
  // Base case
  if (depth == 0 || isTerminalNode()) {
    #ifdef DEBUG  
    Serial.print("----- BASE CASE: ");
    #endif
    
    if (depth == 0) {
      #ifdef DEBUG  
      Serial.print("0 depth. \n");
      #endif
      
      minimaxResult.score = scorePosition(AI_TOKEN);

      #ifdef DEBUG
      char buff[50]; 
      snprintf(buff, sizeof(buff),"SCORE = %d \n", minimaxResult.score);
      Serial.print(buff);
      #endif
      
      return minimaxResult;
    } else {
      if (isWin(AI_TOKEN)) {
        #ifdef DEBUG 
        Serial.print("AI WIN :) \n"); 
        #endif
        
        minimaxResult.score = INFINITY;
      } else if (isWin(USER_TOKEN)){
        #ifdef DEBUG 
        Serial.print("USER WIN :( \n"); 
        #endif
        
        minimaxResult.score = -INFINITY;
      } else {
        #ifdef DEBUG 
        Serial.print("DRAW :/ \n"); 
        #endif
        
        minimaxResult.score = 0;
      }
      return minimaxResult;
    }
  // Maximizing Player case
  } else if (maximizingPlayer) {
    #ifdef DEBUG 
    Serial.print("----- MAXIMIZING PLAYER ----- \n"); 
    #endif
    
    int newMinimaxScore;
    minimaxResult.score = -INFINITY; 
    
    for (byte col = 0; col < COLS; col++) {
      byte row = validateColumn(col); 
      if (row == NOT_VALID) continue;

      board[row][col] = AI_TOKEN;
      newMinimaxScore = (minimax(depth - 1, alpha, beta, false)).score;
      board[row][col] = ' ';
      
      if (newMinimaxScore > minimaxResult.score) {
        #ifdef DEBUG 
        Serial.print("NEW HIGHSCORE FOUND!!\n"); 
        #endif
        
        minimaxResult.score = newMinimaxScore;
        minimaxResult.position = {.row=row, .col=col};  
      }

      alpha = alpha < newMinimaxScore ? newMinimaxScore : alpha;
      if (alpha >= beta) break;
    }
    return minimaxResult;
  // Minimizing player case
  } else {
    #ifdef DEBUG 
    Serial.print("----- MINIMIZING PLAYER ----- \n"); 
    #endif
    
    int newMinimaxScore;
    minimaxResult.score = INFINITY;
     
    for (byte col = 0; col < COLS; col++) {
      byte row = validateColumn(col); 
      if (row == NOT_VALID) continue;
      
      board[row][col] = USER_TOKEN;
      newMinimaxScore = (minimax(depth - 1, alpha, beta, true)).score;
      board[row][col] = ' ';
      
      if (newMinimaxScore < minimaxResult.score) {
        minimaxResult.score = newMinimaxScore;
        minimaxResult.position = {.row=row, .col=col};  
      }

      beta = beta > newMinimaxScore ? newMinimaxScore : beta;
      if (alpha >= beta) break;
    }
    return minimaxResult;
  }
}

void pickBestPosition(char token, Position *pos) {
  int bestScore = -INFINITY;

  for (byte col = 0; col < COLS; col++) {
    
    byte row = validateColumn(col); 
    if (row == NOT_VALID) continue;

    board[row][col] = token;
    int score = scorePosition(token);

    #ifdef DEBUG
    char buff[50]; 
    snprintf(buff, sizeof(buff),"Trying row = %d & col = %d (score = %d)\n", row, col, score);
    Serial.print(buff);
    
    printBoard();
    #endif 
    
    board[row][col] = ' ';


    if(pos->col == EMPTY || score > bestScore){
      bestScore = score;
      pos->col = col;
      pos->row = row;

      #ifdef DEBUG
      char buff[60]; 
      snprintf(buff, sizeof(buff),"New best score found: %d at row = %d & col = %d\n", bestScore, row, col);
      Serial.print(buff);
      #endif
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
    column = getColumnFromKeypad();
    
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
    Serial.print(col+1);
    Serial.print(' ');
  }

  Serial.println();
  
  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS; col++) {
      char buff[3];
      snprintf(buff, sizeof(buff),"|%c", board[row][col]);
      Serial.print(buff);
    }
    Serial.print("| ");
    Serial.println(row);
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
  if (column < 0 || column > 6) return  NOT_VALID;
  for (short row = 5; row >= 0; row--) {
    if (board[row][column] == ' ') return row;
  }

  return  NOT_VALID;
}


bool isWin(char token) {
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
