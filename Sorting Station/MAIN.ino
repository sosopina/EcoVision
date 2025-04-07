#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

// Servos and Sensor
Servo servo1, servo2, servo3;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);

// KY-016 RGB LED Pins
#define RED_LED_PIN 11
#define GREEN_LED_PIN 9
#define BLUE_LED_PIN 10

// Color thresholds
#define RED_THRESHOLD   12500
#define GREEN_THRESHOLD 11500
#define WHITE_THRESHOLD 37000

// Timing
#define RED_DELAY    15000  // 15 seconds
#define GREEN_DELAY  24000  // 24 seconds
#define WHITE_DELAY  5000   // 5 seconds

// Servo configuration
#define POS_PUSH 110
#define POS_IDLE 180
#define SERVO_HOLD 1000  // 1 second push

// Relay Pin
#define RELAY_PIN 7

// State machine
enum SystemState { IDLE, CONVEYOR_MOVING, PUSHING };
SystemState currentState = IDLE;

unsigned long actionStartTime;
uint8_t activeColor = 0;  // 1=Red, 2=Green, 3=White

void setup() {
  // Initialize RGB LED
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  setLED(0, 0, 0);  // Start with LED off

  // Initialize other components
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6);
  resetServos();

  Serial.begin(9600);
  if (!tcs.begin()) {
    Serial.println("Color sensor error!");
    while(1);
  }
}

void loop() {
  switch(currentState) {
    case IDLE:
      checkForBlock();
      break;

    case CONVEYOR_MOVING:
      handleConveyor();
      break;

    case PUSHING:
      handlePushing();
      break;
  }
}

void checkForBlock() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  if (r > RED_THRESHOLD && r > g && r > b) {
    startProcessing(1, RED_DELAY);
    setLED(255, 0, 0);  // Red
  }
  else if (g > GREEN_THRESHOLD && g > r && g > b) {
    startProcessing(2, GREEN_DELAY);
    setLED(0, 255, 0);  // Green
  }
  else if (r > WHITE_THRESHOLD && g > WHITE_THRESHOLD && b > WHITE_THRESHOLD) {
    startProcessing(3, WHITE_DELAY);
    setLED(255, 255, 255);  // White
  }
}

void startProcessing(uint8_t color, unsigned long delay) {
  activeColor = color;
  currentState = CONVEYOR_MOVING;
  actionStartTime = millis();
  digitalWrite(RELAY_PIN, HIGH);
}

void handleConveyor() {
  unsigned long elapsed = millis() - actionStartTime;
  unsigned long requiredDelay = 0;

  switch(activeColor) {
    case 1: requiredDelay = RED_DELAY; break;
    case 2: requiredDelay = GREEN_DELAY; break;
    case 3: requiredDelay = WHITE_DELAY; break;
  }

  if (elapsed >= requiredDelay) {
    currentState = PUSHING;
    actionStartTime = millis();
    activateServo();
  }
}

void activateServo() {
  switch(activeColor) {
    case 1: servo1.write(POS_PUSH); break;
    case 2: servo2.write(POS_PUSH); break;
    case 3: servo3.write(POS_PUSH); break;
  }
}

void handlePushing() {
  if (millis() - actionStartTime >= SERVO_HOLD) {
    resetSystem();
  }
}

void resetSystem() {
  digitalWrite(RELAY_PIN, LOW);
  setLED(0, 0, 0);  // Turn off LED
  resetServos();
  currentState = IDLE;
  activeColor = 0;
}

// LED control function
void setLED(int r, int g, int b) {
  analogWrite(RED_LED_PIN, r);
  analogWrite(GREEN_LED_PIN, g);
  analogWrite(BLUE_LED_PIN, b);
}

void resetServos() {
  servo1.write(POS_IDLE);
  servo2.write(POS_IDLE);
  servo3.write(POS_IDLE);
}
