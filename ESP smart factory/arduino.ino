#include <Servo.h>

// Updated Pins configuration as requested
const int RED_PIN = 2;      // From ESP32 GPIO27
const int GREEN_PIN = 3;    // From ESP32 GPIO26
const int WHITE_PIN = 4;    // From ESP32 GPIO33
const int RELAY_PIN = 5;    // Conveyor relay control
const int RED_SERVO_PIN = 10;
const int GREEN_SERVO_PIN = 11;
const int WHITE_SERVO_PIN = 9;

// Timing configuration
#define RED_DELAY      5000
#define GREEN_DELAY    5000
#define WHITE_DELAY    5000
#define PUSH_HOLD      2000
#define RETURN_HOLD    2000

Servo redServo, greenServo, whiteServo;
enum State { IDLE, CONVEYOR_RUNNING, SERVO_ACTIVE };
State currentState = IDLE;

unsigned long actionStartTime;
uint8_t activeColor = 0;

void setup() {
  // Configure input pins for color signals
  pinMode(RED_PIN, INPUT);
  pinMode(GREEN_PIN, INPUT);
  pinMode(WHITE_PIN, INPUT);
  
  // Configure relay output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  // Initialize servos (detached initially)
  detachAllServos();
  
  Serial.begin(9600);  // For debugging (optional)
}

void loop() {
  switch(currentState) {
    case IDLE:
      checkForColorSignal();
      break;
      
    case CONVEYOR_RUNNING:
      handleConveyor();
      break;
      
    case SERVO_ACTIVE:
      handleServoMovement();
      break;
  }
}

void checkForColorSignal() {
  if (digitalRead(RED_PIN)) {
    startProcessing(1);  // Red
  } else if (digitalRead(GREEN_PIN)) {
    startProcessing(2);  // Green
  } else if (digitalRead(WHITE_PIN)) {
    startProcessing(3);  // White
  }
}

void startProcessing(uint8_t color) {
  activeColor = color;
  currentState = CONVEYOR_RUNNING;
  actionStartTime = millis();
  digitalWrite(RELAY_PIN, HIGH);
  Serial.print("Started processing color: ");
  Serial.println(activeColor);
}

void handleConveyor() {
  unsigned long colorDelay = getColorDelay();
  
  if (millis() - actionStartTime >= colorDelay) {
    currentState = SERVO_ACTIVE;
    actionStartTime = millis();
    digitalWrite(RELAY_PIN, LOW);  // Stop conveyor
    activateServo();
  }
}

void activateServo() {
  switch(activeColor) {
    case 1:  // Red
      redServo.attach(RED_SERVO_PIN);
      redServo.write(90);  // Push position
      break;
    case 2:  // Green
      greenServo.attach(GREEN_SERVO_PIN);
      greenServo.write(90);
      break;
    case 3:  // White
      whiteServo.attach(WHITE_SERVO_PIN);
      whiteServo.write(90);
      break;
  }
  Serial.print("Activated servo for color: ");
  Serial.println(activeColor);
}

void handleServoMovement() {
  unsigned long elapsed = millis() - actionStartTime;

  if (elapsed >= PUSH_HOLD && elapsed < (PUSH_HOLD + RETURN_HOLD)) {
    returnServoToStart();
  }
  
  if (elapsed >= (PUSH_HOLD + RETURN_HOLD)) {
    finishProcessing();
  }
}

void returnServoToStart() {
  switch(activeColor) {
    case 1:
      redServo.write(180);  // Rest position
      break;
    case 2:
      greenServo.write(180);
      break;
    case 3:
      whiteServo.write(180);
      break;
  }
}

void finishProcessing() {
  detachAllServos();
  activeColor = 0;
  currentState = IDLE;
  Serial.println("Processing complete. Ready for next item.");
}

unsigned long getColorDelay() {
  switch(activeColor) {
    case 1: return RED_DELAY;
    case 2: return GREEN_DELAY;
    case 3: return WHITE_DELAY;
    default: return 0;
  }
}

void detachAllServos() {
  redServo.detach();
  greenServo.detach();
  whiteServo.detach();
}
