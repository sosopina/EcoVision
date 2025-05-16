#include <ESP32Servo.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Servo Objects
Servo servo1, servo2, servo3, servo4;

// Color Sensor
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);

// ESP32 GPIO Mapping
const int ARM1_PIN = 4;
const int ARM2_PIN = 13;
const int ARM3_PIN = 12;
const int ARM4_PIN = 14;

// Signaling Pins
const int RED_PIN = 27;
const int GREEN_PIN = 26;
const int WHITE_PIN = 33;
const int DEFECT_PIN = 25;      // To Arduino
const int DEFECT_SENSOR = 32;    // From sensor

// State Machine
enum State { IDLE, ARM_WORKING };
State currentState = IDLE;
uint8_t detectedColor = 0;
bool isDefective = false;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(WHITE_PIN, OUTPUT);
  pinMode(DEFECT_PIN, OUTPUT);
  pinMode(DEFECT_SENSOR, INPUT_PULLDOWN);
  
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(WHITE_PIN, LOW);
  digitalWrite(DEFECT_PIN, LOW);
  
  Wire.begin(21, 22);
  if (!tcs.begin()) while(1);
  
  detachArmServos();
  Serial.begin(115200);
}

void loop() {
  switch(currentState) {
    case IDLE:
      checkColor();
      break;

    case ARM_WORKING:
      executeArmSequence();
      break;
  }
}

void checkColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  if (r > 12500 && r > g && r > b) {
    startProcess(1);
  } 
  else if (g > 11500 && g > r && g > b) {
    startProcess(2);
  } 
  else if (r > 37000 && g > 37000 && b > 37000) {
    startProcess(3);
  }
}

void startProcess(uint8_t color) {
  detectedColor = color;
  isDefective = false;
  currentState = ARM_WORKING;
}

void executeArmSequence() {
  attachArmServos();
  performInitialPickup();
  performAIStation();
  performSorting();
  sendFinalSignal();
  detachArmServos();
  currentState = IDLE;
}

// ======== Arm Movements ======== //
void performInitialPickup() {
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

void performAIStation() {
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
  
   // CORRECTED DEFECT CHECK
  if (digitalRead(DEFECT_SENSOR) == HIGH) {  // Proper condition check
    isDefective = true;
    Serial.println("Defect detected!");
  }
  smoothMove(servo1, 60, 143, 20);
  
  smoothMove(servo2, 100, 93, 20);
  servo3.write(65);
  delay(1000);
  servo2.write(130);
  servo1.write(50);
}
void performSorting() {
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

// ======== Signaling ======== //
void sendFinalSignal() {
  if(isDefective) {
    digitalWrite(DEFECT_PIN, HIGH);
    delay(100);
    digitalWrite(DEFECT_PIN, LOW);
    Serial.println("Sent defect signal");
  } else {
    switch(detectedColor) {
      case 1: 
        digitalWrite(RED_PIN, HIGH);
        delay(100);
        digitalWrite(RED_PIN, LOW);
        break;
      case 2:
        digitalWrite(GREEN_PIN, HIGH);
        delay(100);
        digitalWrite(GREEN_PIN, LOW);
        break;
      case 3:
        digitalWrite(WHITE_PIN, HIGH);
        delay(100);
        digitalWrite(WHITE_PIN, LOW);
        break;
    }
    Serial.print("Sent color signal: ");
    Serial.println(detectedColor);
  }
}

// ======== Helper Functions ======== //
void smoothMove(Servo &servo, int start, int end, int stepDelay) {
  if(start < end) {
    for(int pos = start; pos <= end; pos++) {
      servo.write(pos);
      delay(stepDelay);
    }
  } else {
    for(int pos = start; pos >= end; pos--) {
      servo.write(pos);
      delay(stepDelay);
    }
  }
}

void attachArmServos() {
  servo1.attach(ARM1_PIN);
  servo2.attach(ARM2_PIN);
  servo3.attach(ARM3_PIN);
  servo4.attach(ARM4_PIN);
}

void detachArmServos() {
  servo1.detach();
  servo2.detach();
  servo3.detach();
  servo4.detach();
}
