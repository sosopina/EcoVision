#if defined(ESP32)
#include <ESP32Servo.h>
#else
#include <Servo.h>
#endif
#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Servo Objects
Servo servo1, servo2, servo3, servo4;    // Robotic Arm
Servo servoRed, servoGreen, servoWhite;  // Sorting Station

// ESP32 GPIO Mapping
const int ARM1_PIN = 4;   // Original Arduino Pin 3
const int ARM2_PIN = 13;  // Original Arduino Pin 5
const int ARM3_PIN = 12;  // Original Arduino Pin 6
const int ARM4_PIN = 14;  // Original Arduino Pin 9

const int RED_PIN = 27;    // Original Arduino Pin 3 (Sorting)
const int GREEN_PIN = 26;  // Original Arduino Pin 5 (Sorting)
const int WHITE_PIN = 33;  // Original Arduino Pin 6 (Sorting)
const int RELAY_PIN = 23;  // Original Arduino Pin 7

// Color Sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);

// Timing Configuration
#define RED_DELAY        5000  // Conveyor run time for red
#define GREEN_DELAY      5000  // Conveyor run time for green
#define WHITE_DELAY      5000   // Conveyor run time for white
#define PUSH_HOLD        2000   // 1s hold at 90°
#define RETURN_HOLD      2000   // 1s hold after returning

// State Machine
enum State { IDLE, ARM_WORKING, CONVEYOR_ACTIVE, SERVO_ACTIVE };
State currentState = IDLE;
uint8_t detectedColor = 0;
unsigned long actionStartTime;

// ================== Servo Management ================== //
void detachAllServos() {
  servo1.detach();
  servo2.detach();
  servo3.detach();
  servo4.detach();
  servoRed.detach();
  servoGreen.detach();
  servoWhite.detach();
}

void attachArmServos() {
  servo1.attach(ARM1_PIN);
  servo2.attach(ARM2_PIN);
  servo3.attach(ARM3_PIN);
  servo4.attach(ARM4_PIN);
}

void attachSortingServo() {
  switch(detectedColor) {
    case 1: servoRed.attach(RED_PIN); break;
    case 2: servoGreen.attach(GREEN_PIN); break;
    case 3: servoWhite.attach(WHITE_PIN); break;
  }
}

// ================== Movement Logic ================== //
void smoothMove(Servo &servo, int startPos, int endPos, int speedDelay) {
  if (startPos < endPos) {
    for (int pos = startPos; pos <= endPos; pos++) {
      servo.write(pos);
      delay(speedDelay);
    }
  } else {
    for (int pos = startPos; pos >= endPos; pos--) {
      servo.write(pos);
      delay(speedDelay);
    }
  }
}

void initial_pickup() {
  servo4.write(190);
  servo3.write(110);
  servo1.write(50);
  servo2.write(100);
  delay(3000);
  servo2.write(95);
  delay(1000);
  servo1.write(150);
  servo2.write(105);
  delay(500);
  servo3.write(65);
  delay(1000);
  servo1.write(50);
  delay(500);
  servo2.write(130);
  delay(500);
}

void ai_station() {
  servo4.write(90);
  delay(500);
  servo1.write(50);
  smoothMove(servo2, 130, 100, 20);
  smoothMove(servo1, 50, 155, 20);
  delay(1000);
  servo3.write(110);
  delay(500);
  servo2.write(80);
  smoothMove(servo1, 155, 60, 20);
  smoothMove(servo1, 60, 143, 20);
  smoothMove(servo2, 100, 93, 20);
  servo3.write(65);
  delay(1000);
  servo2.write(130);
  servo1.write(50);
}

void sorting_station() {
  servo2.write(130);
  servo1.write(50);
  delay(2000);
  servo4.write(0);
  delay(2000);
  smoothMove(servo1, 50, 100, 30);
  delay(1000);
  smoothMove(servo2, 130, 115, 40);
  delay(1000);
  servo2.write(110);
  delay(1000);
  servo3.write(110);
  delay(500);
  servo2.write(100);
  servo1.write(50);
  delay(500);
}

// ================== Color Detection ================== //
void checkColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  if (r > 12500 && r > g && r > b) {
    startProcess(1, RED_DELAY);
  } 
  else if (g > 11500 && g > r && g > b) {
    startProcess(2, GREEN_DELAY);
  } 
  else if (r > 37000 && g > 37000 && b > 37000) {
    startProcess(3, WHITE_DELAY);
  }
}

void startProcess(uint8_t color, unsigned long delayTime) {
  detachAllServos();
  attachArmServos();
  detectedColor = color;
  currentState = ARM_WORKING;
}

// ================== Conveyor/Servo Control ================== //
void handleConveyor() {
  unsigned long currentDelay = (detectedColor == 1) ? RED_DELAY : 
                             (detectedColor == 2) ? GREEN_DELAY : WHITE_DELAY;

  if (millis() - actionStartTime >= currentDelay) {
    digitalWrite(RELAY_PIN, LOW);
    activateServo();
    actionStartTime = millis();
    currentState = SERVO_ACTIVE;
  }
}

void activateServo() {
  detachAllServos();
  attachSortingServo();
  
  // Push to 90°
  switch(detectedColor) {
    case 1: servoRed.write(90); break;
    case 2: servoGreen.write(90); break;
    case 3: servoWhite.write(90); break;
  }
}

void handleServo() {
  unsigned long elapsed = millis() - actionStartTime;

  if (elapsed >= PUSH_HOLD && elapsed < (PUSH_HOLD + RETURN_HOLD)) {
    // Return to 180° after 1s
    switch(detectedColor) {
      case 1: servoRed.write(180); break;
      case 2: servoGreen.write(180); break;
      case 3: servoWhite.write(180); break;
    }
  }

  if (elapsed >= (PUSH_HOLD + RETURN_HOLD)) {
    detachAllServos();
    currentState = IDLE;
    detectedColor = 0;
  }
}

// ================== Setup ================== //
void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  Wire.begin(21, 22);  // SDA=GPIO21, SCL=GPIO22
  if (!tcs.begin()) while(1);
  
  detachAllServos();
}

// ================== Main Loop ================== //
void loop() {
  switch(currentState) {
    case IDLE:
      checkColor();
      break;

    case ARM_WORKING:
      initial_pickup();
      ai_station();
      sorting_station();
      detachAllServos();
      digitalWrite(RELAY_PIN, HIGH);
      actionStartTime = millis();
      currentState = CONVEYOR_ACTIVE;
      break;

    case CONVEYOR_ACTIVE:
      handleConveyor();
      break;

    case SERVO_ACTIVE:
      handleServo();
      break;
  }
}
