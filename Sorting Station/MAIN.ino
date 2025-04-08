#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

// Servos and Sensor
Servo servo1, servo2, servo3;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_600MS, TCS34725_GAIN_1X);

// RGB LED Pins
#define RED_LED_PIN 11
#define GREEN_LED_PIN 9
#define BLUE_LED_PIN 10

// Color thresholds (keep your values)
#define RED_THRESHOLD   12500
#define GREEN_THRESHOLD 11500
#define WHITE_THRESHOLD 37000

// Relay Pin
#define RELAY_PIN 7

// Timing
#define CHECK_INTERVAL 2000  // 2 seconds between readings

unsigned long lastCheck = 0;

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  setLED(0, 0, 0);

  servo1.attach(3);
  servo2.attach(5);
  servo3.attach(6);
  resetServos();

  Serial.begin(9600);
  if (!tcs.begin()) {
    Serial.println("Sensor error!");
    while(1);
  }
}

void loop() {
  if (millis() - lastCheck >= CHECK_INTERVAL) {
    checkColor();
    lastCheck = millis();
  }
}

void checkColor() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);
  
  if (r > RED_THRESHOLD && r > g && r > b) {
    processColor(1, 255, 0, 0);  // Red
  } 
  else if (g > GREEN_THRESHOLD && g > r && g > b) {
    processColor(2, 0, 255, 0);  // Green
  } 
  else if (r > WHITE_THRESHOLD && g > WHITE_THRESHOLD && b > WHITE_THRESHOLD) {
    processColor(3, 255, 255, 255);  // White
  }
  else {
    Serial.println("Unknown color");
    resetServos();
  }
}

void processColor(uint8_t color, int ledR, int ledG, int ledB) {
  setLED(ledR, ledG, ledB);
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);  // Relay ON time

  // Move servo
  switch(color) {
    case 1: servo1.write(90); break;
    case 2: servo2.write(90); break;
    case 3: servo3.write(90); break;
  }
  
  delay(2000);  // Hold servo position
  
  // Reset
  resetServos();
  digitalWrite(RELAY_PIN, LOW);
  setLED(0, 0, 0);
}

void resetServos() {
  servo1.write(180);
  servo2.write(180);
  servo3.write(180);
}

void setLED(int r, int g, int b) {
  analogWrite(RED_LED_PIN, r);
  analogWrite(GREEN_LED_PIN, g);
  analogWrite(BLUE_LED_PIN, b);
}
