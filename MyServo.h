/*
 * @file MyServo.h
 * @brief Extension of Servo.h to include project macros related to the SERVO and some simple functions
 */


#ifndef MYSERVO_HEADER
#define MYSERVO_HEADER

#include <Servo.h>

#define SERVO_PIN 7

#define LEFT_MAGAZINE_ANGLE   105
#define DEPLOY_ANGLE          88
#define RIGHT_MAGAZINE_ANGLE  70

#define DEPLOY_DELAY          2200
#define MAGAZINE_SHIFT_DELAY  500

Servo servo;


/*
 * @brief Setup funtion. Used it in setup() in the main file. 
 */
void setupServo() {
  servo.attach(SERVO_PIN);
}

/*
 * @brief Deploy a token to be deployed
 */
void deploy() {
  delay(MAGAZINE_SHIFT_DELAY);
  servo.write(DEPLOY_ANGLE);
  delay(DEPLOY_DELAY);
}

/*
 * @brief Select tokens from left magazine to be deployed
 */
void selectLeftToken() {
  servo.write(LEFT_MAGAZINE_ANGLE);
  deploy();
}

/*
 * @brief Select tokens from right magazine to be deployed 
 */
void selectRightToken() {
  servo.write(RIGHT_MAGAZINE_ANGLE);
  deploy();
}

#endif
