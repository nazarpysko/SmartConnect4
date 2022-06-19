/*
 * @file DeploySystem.h
 * @brief Contains all the logic for, selecting and deploying tokens to the board.
 * Basically, combines all the functionality of LimitSwitch.h, MyServo.h and MyStepper.h libraries.
 * 
 * User tokens will be placed in left magazine and AI tokens in the rigth magazine
 */

#ifndef DEPLOYSYSTEM_HEADER
#define DEPLOYSYSTEM_HEADER

#include "LimitSwitch.h"
#include "MyServo.h"
#include "MyStepper.h"

// TODO: Redefine player tokens in order to not declaring in different files
#define USER_TOKEN  'x'
#define AI_TOKEN    'o'


/*
 * @brief Deploy chosen token into chosen column. 
 */
void deployToken(char token, byte column) {
  setColumn(column);
  
  if (token == USER_TOKEN) {
    selectLeftToken();    
  } else {
    selectRightToken();
  }
}

#endif
