/*
 * @file LimitSwitch.h
 * @brief Contains all functionality needed of a LIMIT SWITCH for this project.
 */

#ifndef LIMITSWITCH_HEADER
#define LIMITSWITCH_HEADER

#define LIMITSWITCH_PIN 13


/*
 * @brief Setup funtion. Used it in setup() in the main file. 
 */

void setupLimitSwitch() {
  pinMode(LIMITSWITCH_PIN, INPUT);
}


/*
* @brief LIMIT SWITCH is configured in normally open state. 
* @return LIMIT SWITCH state. 
*/
int getLimitSwitchState(){
  if(digitalRead(LIMITSWITCH_PIN) == LOW){
    return 0;
  }
  else{
    return 1;
  }  
}

#endif
