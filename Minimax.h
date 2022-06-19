/*
 * @file Minimax.h
 * @brief Minimax implementation in arduino.
 */

#ifndef MIMNIMAX_HEADER
#define MIMNIMAX_HEADER

#include "Board.h"

// Score constants
// OPEN means there is still chance to make 4 in a row
#define FOUR_IN_A_ROW_SCORE 100
#define OPEN_THREE_IN_A_ROW_SCORE 5
#define OPEN_TWO_IN_A_ROW_SCORE 2
#define OPEN_ODD_THREE_IN_A_ROW_SCORE 80
#define CENTER_SCORE 3


typedef struct {
  byte row;
  byte col;
} Position;

typedef struct {
  byte token;
  byte oddToken; 
  byte empty;
} AccountTokens;


typedef struct {
  int score; 
  Position position;
} MinimaxResult;


/*
 * @brief Simple function which counts tokens of a board window
 */
void countTokens(char *window, char token, AccountTokens *at) {
  for (byte i = 0; i < 4; i++)
    if (window[i] == token) at->token++;
    else if (window[i] == ' ') at->empty++;
    else at->oddToken++;
}


/*
 * @brief Scores a group of 4 consecutive boards spots 
 */
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


/*
 * @brief Return score of a board state
 */
int scorePosition(char token) {
  int score = 0;

  // Score center positions
  for (byte row = 0; row < ROWS-3; row++) {
    // Create a group of central positions of board to check
    char window[4] = 
    { getFromBoard(row, 3), 
      getFromBoard(row+1, 3),
      getFromBoard(row+2, 3),
      getFromBoard(row+3, 3)};
    
    AccountTokens at = {0, 0, 0};
    countTokens(window, token, &at);
    score += at.token * CENTER_SCORE;
  }
  
  // Score horizontal positions
  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS - 3; col++) {
      // Create a group of horizontal positions of board to check
      char window[4] = 
      { getFromBoard(row, col),
        getFromBoard(row, col+1),
        getFromBoard(row, col+2),
        getFromBoard(row, col+3)};
        
      score += evaluateWindow(window, token);
    }
  }

  // Score vertical positions
  for (byte col = 0; col < COLS; col++) {
    for (byte row = 0; row < ROWS - 3; row++) {
      // Create a group of vertical positions of board to check
      char window[4] = 
      { getFromBoard(row, col),
        getFromBoard(row+1, col),
        getFromBoard(row+2, col),
        getFromBoard(row+3, col)};
        
      score += evaluateWindow(window, token);
    }
  }

  // Score positive diagonal
  for (byte row = 0; row < ROWS - 3; row++) {
    for (byte col = 0; col < COLS - 3; col++) {
      // Create a group of positive diagonal positions of board to check
      char window[4] = 
      { getFromBoard(row, col),
        getFromBoard(row+1, col+1),
        getFromBoard(row+2, col+2),
        getFromBoard(row+3, col+3)};
        
      score += evaluateWindow(window, token);
    }
  }

  // Score negative diagonal
  for (byte row = 0; row < ROWS - 3; row++) {
    for (byte col = COLS - 4; col < COLS; col++) {
      // Create a group of negative diagonal positions of board to check
      char window[4] = 
      { getFromBoard(row, col),
        getFromBoard(row+1, col-1),
        getFromBoard(row+2, col-2),
        getFromBoard(row+3, col-3)};

      score += evaluateWindow(window, token);
    }
  }
  
  return score;
}

/*
 * @brief Checks if the current board state is a terminal node
 */
bool isTerminalNode(void) {
  return isBoardFull() || isWin(USER_TOKEN) || isWin(AI_TOKEN);
}

/*
 * @brief Minimax algorithm implementation
 */
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

      setBoard(AI_TOKEN, row, col);
      newMinimaxScore = (minimax(depth - 1, alpha, beta, false)).score;
      setBoard(' ', row, col);
      
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

      setBoard(USER_TOKEN, row, col);
      newMinimaxScore = (minimax(depth - 1, alpha, beta, true)).score;
      setBoard(' ', row, col);
      
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

#endif
