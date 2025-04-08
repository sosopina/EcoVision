#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

// Servos and Sensor
Servo servo1, servo2, servo3;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);

// Timing Configuration
#define RED_DELAY    14000  // 14 seconds conveyor
#define GREEN_DELAY  20000  // 20 seconds conveyor
#define WHITE_DELAY  5000   // 5 seconds conveyor
#define SERVO_HOLD   2000   // 2 seconds servo movement

// Hardware Pins
#define RELAY_PIN 7

// System States
enum State { IDLE, WAIT_FOR_ARM, CONVEYOR_ACTIVE, SERVO_ACTIVE };
State currentState = IDLE;

// Control Variables
unsigned long actionStartTime;
uint8_t detectedColor = 0; // 1=Red, 2=Green, 3=White

void setup() {
  Serial.begin(9600); // Initialize serial communication
  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6);
  pinMode(RELAY_PIN, OUTPUT);
  resetSystem();

  if (!tcs.begin()) {
    while(1); // Freeze if sensor fails
  }
}

void loop() {
  switch(currentState) {
    case IDLE:
      checkColor(); // Continuously check for colors
      break;

    case WAIT_FOR_ARM:
      waitForArmCompletion(); // Wait for robotic arm to finish
      break;

    case CONVEYOR_ACTIVE:
      handleConveyor(); // Run conveyor belt
      break;

    case SERVO_ACTIVE:
      handleServo(); // Activate servo
      break;
  }
}

// Color detection logic
void checkColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  if (r > 12500 && r > g && r > b) { // Red
    startProcess(1, RED_DELAY);
  } 
  else if (g > 11500 && g > r && g > b) { // Green
    startProcess(2, GREEN_DELAY);
  } 
  else if (r > 37000 && g > 37000 && b > 37000) { // White
    startProcess(3, WHITE_DELAY);
  }
}

// Start the process after color detection
void startProcess(uint8_t color, unsigned long delayTime) {
  detectedColor = color;
  Serial.write('S'); // Signal robotic arm to start
  currentState = WAIT_FOR_ARM; // Wait for arm to complete
}

// Wait for 'D' confirmation from Arduino 1
void waitForArmCompletion() {
  if (Serial.available() && Serial.read() == 'D') {
    digitalWrite(RELAY_PIN, HIGH); // Start conveyor
    actionStartTime = millis();
    currentState = CONVEYOR_ACTIVE;
  }
}

// Handle conveyor timing
void handleConveyor() {
  unsigned long currentDelay = 0;
  switch(detectedColor) {
    case 1: currentDelay = RED_DELAY; break;
    case 2: currentDelay = GREEN_DELAY; break;
    case 3: currentDelay = WHITE_DELAY; break;
  }

  if (millis() - actionStartTime >= currentDelay) {
    digitalWrite(RELAY_PIN, LOW); // Stop conveyor
    activateServo();
    actionStartTime = millis();
    currentState = SERVO_ACTIVE;
  }
}

// Activate servo based on color
void activateServo() {
  switch(detectedColor) {
    case 1: servo1.write(90); break;
    case 2: servo2.write(90); break;
    case 3: servo3.write(90); break;
  }
}

// Handle servo reset after delay
void handleServo() {
  if (millis() - actionStartTime >= SERVO_HOLD) {
    resetServos();
    currentState = IDLE;
    detectedColor = 0;
  }
}

// Reset servos to default position
void resetServos() {
  servo1.write(180);
  servo2.write(180);
  servo3.write(180);
}

// Full system reset
void resetSystem() {
  digitalWrite(RELAY_PIN, LOW);
  resetServos();
}
