#include <Servo.h>

Servo servo1;  // Servo on pin 3
Servo servo2;  // Servo on pin 5
Servo servo3;  // Servo on pin 6
Servo servo4;  // Servo on pin 9

// Function to move servo smoothly
void smoothMove(Servo &servo, int startPos, int endPos, int speedDelay) {
    if (startPos < endPos) {
        for (int pos = startPos; pos <= endPos; pos++) {
            servo.write(pos);
            delay(speedDelay);  // Speed control for smooth movement
        }
    } else {
        for (int pos = startPos; pos >= endPos; pos--) {
            servo.write(pos);
            delay(speedDelay);  // Speed control for smooth movement
        }
    }
}

// Function for Initial Pick-Up
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

// Function for AI Station placement
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

// Function for Sorting Station placement
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

void setup() {
    servo1.attach(3);
    servo2.attach(5);
    servo3.attach(6);
    servo4.attach(9);
}

void loop() {
    initial_pickup();   // Perform initial pick-up
    ai_station();       // Move to AI station for inspection
    sorting_station();  // Place cube in sorting zone
}
