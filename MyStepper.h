/*
 * @file MyStepper.h
 * @brief This file handles all related to the STEP MOTOR, which is used to control the COLUMN SELECTOR. 
 * There is a LIMIT SWITCH involved which detects if the COLUMN SELECTOR is fully extended. This means that the 
 * COLUMN SELECTOR is currently aiming to the first column of the board. 
 * 
 * The STEP MOTOR model is 28byj-48 and include the ULN2003 driver.
 */

#ifndef MY_STEPPER_HEADER
#define MY_STEPPER_HEADER

#include "LimitSwitch.h"

/* STEP MOTOR defines ---------------------------------- */
#define MOTOR_PIN_1 8        // ULN2003 In1
#define MOTOR_PIN_2 9        // ULN2003 In2
#define MOTOR_PIN_3 10       // ULN2003 In3
#define MOTOR_PIN_4 11       // ULN2003 In4

#define MOTOR_SPEED 1250
#define STEPS_PER_COLUMN 125
#define REFERENCE_STEPS 15 // Used to adjust distance COLUMN SELECTOR and LIMIT SWITCH

/* Global variables for STEP MOTOR ---------------------- */
int motorLookup[8] = {B01001, B00001, B00011, B00010, B00110, B00100, B01100, B01000}; // STEP MOTOR steps sequence
byte currentColumn = 0;


/*
 * @brief Setup funtion. Used it in setup() in the main file. 
 */
void setupStepMotor() {
  pinMode(MOTOR_PIN_1, OUTPUT); 
  pinMode(MOTOR_PIN_2, OUTPUT); 
  pinMode(MOTOR_PIN_3, OUTPUT); 
  pinMode(MOTOR_PIN_4, OUTPUT);
}

/*
 * @brief Aux. function for rotating the STEP MOTOR
 */
void setOutput(int out){ 
  digitalWrite(MOTOR_PIN_1, bitRead(motorLookup[out], 0)); 
  digitalWrite(MOTOR_PIN_2, bitRead(motorLookup[out], 1)); 
  digitalWrite(MOTOR_PIN_3, bitRead(motorLookup[out], 2)); 
  digitalWrite(MOTOR_PIN_4, bitRead(motorLookup[out], 3)); 
}


/*
* @brief Handles STEP MOTOR rotation in "turn" direction for "steps" amount of steps
* @param turn   Moving direction of COLUMN SELECTOR. 'R' moves COLUMN SELECTOR to the rigth and 'L' moves COLUMN SELECTOR to the left
* @param steps  Amount of steps to rotate
*/
void rotate(char turn, int steps){
  for(int j = 0; j < steps; j++){
    if(turn == 'R'){
      for(int i = 0; i < 8; i++){ 
        setOutput(i); 
        delayMicroseconds(MOTOR_SPEED); 
      } 
    }
    if(turn == 'L'){
      for(int i = 7; i >= 0; i--){ 
        setOutput(i); 
        delayMicroseconds(MOTOR_SPEED); 
      } 
    }
  }
  digitalWrite(MOTOR_PIN_1, 0); 
  digitalWrite(MOTOR_PIN_2, 0); 
  digitalWrite(MOTOR_PIN_3, 0); 
  digitalWrite(MOTOR_PIN_4, 0); 
}


/*
 * @brief Adjust the COLUMN SELECTOR to aim the first column
 */
void referenceSelector(){
  Serial.println("[SYSTEM]: Referencing...");
    
  while(getLimitSwitchState() == 1){
    rotate('L', 1);
  }
  delay(50);
  rotate('R', REFERENCE_STEPS);
  currentColumn = 0;

  Serial.println("[SYSTEM]: Referencing DONE");
}

/*
 * @brief Move the COLUMN SELECTOR to the chosen column. Also updates the currentColumn variable. 
 * @param column Chosen column
 */
void setColumn(byte column) {
  int columnsToMove = currentColumn - column;
  char direction;
  
  if (columnsToMove == 0) return; // No need to move COLUMN SELECTOR

  if (column > currentColumn) direction = 'R';  // Move COLUMN SELECTOR to the right
  else direction = 'L';                         // Move COLUMN SELECTOR to the left

  rotate(direction, STEPS_PER_COLUMN * abs(columnsToMove));    
  currentColumn = column;
}

#endif
