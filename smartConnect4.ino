#include "DeploySystem.h"
#include "MyServo.h"
#include "MyStepper.h"

#include "MyDisplay.h"
#include "MyKeypad.h"

#include "Board.h"

#include "Minimax.h"

//#define DEBUG
#define MINIMAX // Comment if you don't want to play with minimax algorithm



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

  // Initialize board
  referenceSelector();
}

void loop() {
  switch()
  playGame();  
  for(;;); // Dummy loop to end the game
}



/*
 * @brief Handles AI turn
 */
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


/*
 * @brief Handles user turn
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

/*
 * @brief Plays connect 4 until the game ends
 */
void playGame() {
  byte tokensDeployed = 0;
  while(tokensDeployed < 41){
    userTurn();
    if (isWin(USER_TOKEN)) break;
    AITurn();
    if (isWin(AI_TOKEN)) break;
    
    tokensDeployed++;
  }
}
