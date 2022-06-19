/*
 * @file SimpleStrategy.h
 * @brief Implementation of a simple strategy to win at connect 4
 */

#ifndef SIMPLESTRATEGY_HEADER
#define SIMPLESTRATEGY_HEADER

/*
 * @brief Simple algorithm which deploys best column to play
 */
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

#endif
