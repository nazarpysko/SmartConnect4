/*
 * @file Board.h
 * @brief Contains board variable and some useful functions.
 */

#ifndef BOARD_HEADER
#define BOARD_HEADER

#include "DeploySystem.h"

#define ROWS 6
#define COLS 7

#define USER_TOKEN  'x'
#define AI_TOKEN    'o'

#define EMPTY 10
#define NOT_VALID 8

char board[ROWS][COLS] = {
//#COLS:  0    1    2    3    4    5    6    #ROWS
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 0
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 1
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 2
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 3
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 4
        {' ', ' ', ' ', ' ', ' ', ' ', ' '}, // 5
};


void restartBoard() {
  memset(board, ' ', sizeof(board));
}

/*
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


/*
 * @brief Get function to read board variable
*/
char getFromBoard(byte row, byte column) {
  return board[row][column];
}

/*
 * @brief Set function to modify board variable
*/
void setBoard(char token, byte row, byte column) {
  board[row][column] = token;
}

/**
 * @brief Updates physical and virtual board and additionally prints stuff for debugging
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

    deployToken(token, column);
}


/*
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

/*
 * @brief Check board for 'token' wins  
 * @param token Token to be checked
 * @return True if win for 'token' is found and false otherwise
 */
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

/*
 * @brief Returns true if the board is full of tokens and false otherwise
 */
bool isBoardFull(void) {
  bool boardFull = false;
  for (byte col = 0; col < COLS; col++) {
    // If not empty spot found, board is not empty 
    if (board[0][col] == ' ') break;  
    
    // If all columns checked and still here, board is full
    if (col == COLS - 1) boardFull = true; 
  }

  return boardFull;
}

#endif
