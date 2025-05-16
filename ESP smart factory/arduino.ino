#include <Servo.h>

// Pin Configuration
const int RED_PIN = 2;
const int GREEN_PIN = 3;
const int WHITE_PIN = 4;
const int DEFECT_PIN = 7;
const int RELAY_PIN = 5;
const int RED_SERVO_PIN = 10;
const int GREEN_SERVO_PIN = 11;
const int WHITE_SERVO_PIN = 9;

// Timing Configuration
#define DEFECT_DELAY   5000
#define RED_DELAY      5000
#define GREEN_DELAY    5000
#define WHITE_DELAY    5000
#define PUSH_HOLD      2000
#define RETURN_HOLD    2000

Servo redServo, greenServo, whiteServo;
enum State { IDLE, CONVEYOR_RUNNING, SERVO_ACTIVE, DEFECT_PROCESSING };
State currentState = IDLE;

unsigned long actionStartTime;
uint8_t activeColor = 0;

void setup() {
  // Initialize pins
  pinMode(RED_PIN, INPUT);
  pinMode(GREEN_PIN, INPUT);
  pinMode(WHITE_PIN, INPUT);
  pinMode(DEFECT_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Initialize servos in hold position
  redServo.attach(RED_SERVO_PIN);
  greenServo.attach(GREEN_SERVO_PIN);
  whiteServo.attach(WHITE_SERVO_PIN);
  returnServosToStart();
  
  digitalWrite(RELAY_PIN, LOW);
  Serial.begin(9600);
}

void loop() {
  switch(currentState) {
    case IDLE:
      checkForSignals();
      break;
      
    case CONVEYOR_RUNNING:
      handleConveyor();
      break;
      
    case SERVO_ACTIVE:
      handleServoMovement();
      break;
      
    case DEFECT_PROCESSING:
      handleDefective();
      break;
  }
}

void checkForSignals() {
  returnServosToStart();
  
  if (digitalRead(DEFECT_PIN)) {
    startDefectiveProcessing();
  }
  else if (digitalRead(RED_PIN)) {
    startColorProcessing(1);
  }
  else if (digitalRead(GREEN_PIN)) {
    startColorProcessing(2);
  }
  else if (digitalRead(WHITE_PIN)) {
    startColorProcessing(3);
  }
}

void returnServosToStart() {
  redServo.write(180);
  greenServo.write(180);
  whiteServo.write(180);
}

void startDefectiveProcessing() {
  currentState = DEFECT_PROCESSING;
  actionStartTime = millis();
  digitalWrite(RELAY_PIN, HIGH);
}

void startColorProcessing(uint8_t color) {
  activeColor = color;
  currentState = CONVEYOR_RUNNING;
  actionStartTime = millis();
  digitalWrite(RELAY_PIN, HIGH);
}

void handleConveyor() {
  unsigned long colorDelay = getColorDelay();
  
  if (millis() - actionStartTime >= colorDelay) {
    currentState = SERVO_ACTIVE;
    actionStartTime = millis();
    digitalWrite(RELAY_PIN, LOW);
    activateServo();
  }
}

void activateServo() {
  switch(activeColor) {
    case 1: redServo.write(90); break;
    case 2: greenServo.write(90); break;
    case 3: whiteServo.write(90); break;
  }
}

void handleServoMovement() {
  unsigned long elapsed = millis() - actionStartTime;

  if (elapsed >= PUSH_HOLD && elapsed < (PUSH_HOLD + RETURN_HOLD)) {
    returnServosToStart();
  }
  
  if (elapsed >= (PUSH_HOLD + RETURN_HOLD)) {
    finishProcessing();
  }
}

void handleDefective() {
  if (millis() - actionStartTime >= DEFECT_DELAY) {
    digitalWrite(RELAY_PIN, LOW);
    currentState = IDLE;
  }
}

void finishProcessing() {
  returnServosToStart();
  activeColor = 0;
  currentState = IDLE;
}

unsigned long getColorDelay() {
  switch(activeColor) {
    case 1: return RED_DELAY;
    case 2: return GREEN_DELAY;
    case 3: return WHITE_DELAY;
    default: return 0;
  }
}
