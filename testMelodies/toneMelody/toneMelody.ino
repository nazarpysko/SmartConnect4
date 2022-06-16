#include "pitches.h"

#define BUZZER_PIN 8

//int melody[] = {
//  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5, 
//  NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, 
//  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5, 
//  NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
//};
//
//// The note duration, 8 = 8th note, 4 = quarter note, etc.
//int durations[] = {
//  8, 8, 8, 4, 4, 4, 
//  4, 5, 8, 8, 8, 8, 
//  8, 8, 8, 4, 4, 4, 
//  4, 5, 8, 8, 8, 8
//};
//// determine the length of the arrays to use in the loop iteration
//int songLength = sizeof(melody)/sizeof(melody[0]);

void setup() {
  alertUserLooser();   
}

void loop() {
  
  // Iterate through both arrays
  // Notice how the iteration variable thisNote is created in the parenthesis
  // The for loop stops when it is equal to the size of the melody array
//  for (int thisNote = 0; thisNote < songLength; thisNote++){
//    // determine the duration of the notes that the computer understands
//    // divide 1000 by the value, so the first note lasts for 1000/8 milliseconds
//    int duration = 1000 / durations[thisNote];
//    tone(BUZZER_PIN, melody[thisNote], duration);
//    // pause between notes
//    int pause = duration * 1.3;
//    delay(pause);
//    // stop the tone
//    noTone(BUZZER_PIN);
//  }
}

void playMelody(int durations[], int melody[], int songLength){
  for (int i = 0; i < songLength; i++) {
    int duration = 1000 / durations[i];
    tone(BUZZER_PIN, melody[i], duration);
    // pause between notes
    int pause = duration * 1.3;
    delay(pause);
    // stop the tone
    noTone(BUZZER_PIN);
  }
}

void alertUserTurn(){
  int durations[] = {10, 8, 5};
  int melody[] = {NOTE_D5, NOTE_D5, NOTE_FS5};
  int songLength = sizeof(melody)/sizeof(melody[0]);  
  
  playMelody(durations, melody, songLength);
}

void alertUserWinner() {
  int durations[] = {10, 8, 5};
  int melody[] = {NOTE_D5, NOTE_D5, NOTE_FS5};
  int songLength = sizeof(melody)/sizeof(melody[0]);  
  
  playMelody(durations, melody, songLength);
}

void alertUserLooser() {
//  int durations[] = {
//    12, 12, 4, 
//    16, 16, 16, 16, 16, 16,
//    8, 8, 4};

  int durations[] = {
    6, 6, 2, 
    8, 8, 8, 8, 8, 8,
    4, 4, 2};
  
  int melody[] = {
    NOTE_C5, NOTE_G4, NOTE_E4, 
    NOTE_A4, NOTE_B4, NOTE_A4, NOTE_GS4, NOTE_AS4, NOTE_GS4,
    NOTE_G4, NOTE_D4, NOTE_E4};
  
  int songLength = sizeof(melody)/sizeof(melody[0]);  
  
  playMelody(durations, melody, songLength);
}
