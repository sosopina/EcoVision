# EcoVision Robotic Arm

## Overview
EcoVision is a 4-DOF robotic arm designed to automate pick-and-place tasks for a smart factory prototype. It uses four SG90 servos to move in different directions and is programmed using an Arduino Uno. The robot performs object pickup, placement into an AI inspection station, and then moves the item to a sorting station. This robotic arm simulates basic industrial automation using affordable and accessible components, making it ideal for educational and prototyping purposes.

## Hardware Used
- **4x SG90 Servos** – for base rotation, shoulder, elbow, and gripper movements
- **Arduino Uno** – controls the servo motors with PWM signals
- **Breadboard** – for easy connection of the servo motor signal wires and power distribution
- **Jumper wires / Male-to-male Dupont cables** – used to connect the servos to the Arduino via the breadboard
- **Plastic robotic arm frame** – pre-cut and screwed together for assembly
  - Product link: [Plastic Robotic Arm from AliExpress](https://www.aliexpress.com/item/1005005803660763.html?spm=a2g0o.cart.0.0.508138daiSqR1W&mp=1&pdp_npi=5%40dis%21MAD%21MAD%20310.51%21MAD%20276.35%21%21%21%21%21%40211b81a317440338920366152e9d30%2112000034422040564%21ct%21MA%213043355265%21%211%210)

## Servo Pin Assignments
- `servo1` → Arm shoulder (pin 3)
- `servo2` → Arm elbow (pin 5)
- `servo3` → Gripper (pin 6)
- `servo4` → Base rotation (pin 9)

## Program Behavior

### 1. Initial Pick-Up Sequence
The robotic arm performs the following:
- Positions itself over a cube
- Adjusts joints to prepare for gripping
- Closes the gripper to secure the cube
- Lifts the cube for transportation

### 2. AI Station Placement
- Rotates the base to the AI station position
- Adjusts the arm and lowers it into inspection position
- Releases the cube carefully
- Waits for the AI station to check the quality of the cube before proceeding

### 3. Sorting Station Placement
- Rotates the base to the sorting station
- Adjusts arm joints into placement configuration
- Places the cube on the conveyor to be sorted later

## Code
The Arduino code controls each servo by writing specific angles, using both smooth and instant transitions. Movements are sequenced to ensure coordination between joints and realistic operation. The complete code is included in a separate `.ino` file in this repository.

