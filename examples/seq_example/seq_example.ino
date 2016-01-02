
#include "sequencer.h"

#define DROP_PIN 3
#define PISTON_PIN 4
#define CAM_PIN 5
#define START_BTN 10

// Function declarations. Implementations are below.
void dropOn();
void dropOff();
void onDropsDone();
void pistonOn();
void pistonOff();
void camOn();
void camOff();

enum State {
  WAITING,
  RUNNING
};
State state = WAITING;

SequenceEvent SEQ1_EVENTS[] = {
  {0, &dropOn},
  {10000 /* us */, &dropOff},
  {10000, &dropOn},  
  {10000, &dropOff},
  {0, &onDropsDone}
};
Sequence seq1(SEQ1_EVENTS, 5);

SequenceEvent SEQ2_EVENTS[] = {
  {100000, &pistonOn},
  {10000, &pistonOff}
};
Sequence seq2(SEQ2_EVENTS, 2);

SequenceEvent SEQ3_EVENTS[] = {
  {120000, &camOn},
  {10000, &camOff}
};
Sequence seq3(SEQ3_EVENTS, 2);

Sequence* sequences[] = {&seq1, &seq2, &seq3}; // Array of pointers to Sequence.
SequenceHelper seqControl(sequences, 3);

void setup() {
  pinMode(DROP_PIN, OUTPUT);
  pinMode(CAM_PIN, OUTPUT);
  pinMode(PISTON_PIN, OUTPUT);
  pinMode(START_BTN, INPUT_PULLUP);
  Serial.begin(9600);  
}

void loop() {

  // Declare vars outside switch block or face "error: jump to case label"
  bool btnPressed; 
  switch (state) {
    case WAITING:
      // TODO: Read serial port
      
      // Wait for button pressed.
      btnPressed = true; // TODO: implement
      if (btnPressed) {
        seq1.start();
        state = RUNNING;
      }
      break;

    case RUNNING:
      seqControl.tick();
      
      if (seqControl.done()) {
        state = WAITING;  
      }
      break;
  }
}

void onDropsDone() {
  seq2.start();
  seq3.start();
}

void dropOn() {
  digitalWrite(DROP_PIN, HIGH);
}

void dropOff() {
  digitalWrite(DROP_PIN, LOW);
}

void pistonOn() {
  digitalWrite(PISTON_PIN, HIGH);
}

void pistonOff() {
  digitalWrite(PISTON_PIN, LOW);
}

void camOn() {
  digitalWrite(CAM_PIN, HIGH);
}

void camOff() {
  digitalWrite(CAM_PIN, LOW);
}


