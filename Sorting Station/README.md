
# EcoVision - Color Sorting Station

## Overview
The Color Sorting Station is a key component of the EcoVision Smart Factory. This station is responsible for sorting LEGO-like cubes based on their color and directing them into designated paths for further processing. The station is now located at the end of the process, where the robotic arm picks up cubes from the quality control station and places them on the conveyor for sorting. The cubes are sorted into four categories: Red, Green, White, and Defective.

## How It Works

- **Manual Cube Placement**: The user places a cube at the start of the conveyor belt.
- **Color Detection**: A Keyestudio TCS34725 color sensor reads the color of the cube by looking at its side.
- **Data Processing**: The Arduino Uno processes the color data and determines which servo should activate.
- **Conveyor Movement**: The conveyor moves the cube forward.
- **Servo Activation**: The designated servo (SG90 9g Micro Servo) pushes the cube onto its assigned path.
- **Sorted Cube Storage**: The cubes are sorted into four groups: Red, Green, White, and Defective.

## Components & Their Functions

1. **Keyestudio TCS34725 Color Sensor**
   - Detects the RGB values of the cube’s side.
   - Works best when the cube is close to the sensor to minimize external light interference.
   - The sensor continuously reads colors and sends the data to the Arduino.

2. **Arduino Uno**
   - The main controller that processes sensor data and decides which servo to activate.
   - Handles timing and logic for servo movements.

3. **Conveyor Belt**
   - Length: 52 cm
   - Powered by a 20V 3.25A Lenovo laptop charger.
   - Moves at a slow speed, ensuring accurate sorting.

4. **SG90 9g Micro Servos (x3)**
   - Placed at different points along the conveyor.
   - Each servo corresponds to a specific color.
   - Equipped with small sticks that push the cubes into their respective paths.

5. **RGB LED Module**
   - Provides visual feedback during sorting.
   - Changes color to indicate system status.

6. **Relay Module (Songle 10A 250V SRD 05VDC SL C)**
   - Controls the power supply to the conveyor belt, allowing the Arduino to manage its operation.

7. **Power Supply**
   - 20V Lenovo Laptop Charger: Powers the conveyor belt.
   - 5V Power Supply: Powers the servos.
   - Separate 5V Supply for Arduino: Ensures stable operation.

## Code & Logic Breakdown

1. **Color Detection & Classification**
   - The sensor continuously reads RGB values.
   - If no cube is detected, the system registers the color as "unknown."
   - The system assigns the cube to Red, Green, White, or Defective based on predefined thresholds.

2. **Servo Control & Timing**
   - The Arduino determines which servo should activate.
   - Each servo is positioned at a different point, so cubes must travel a certain distance before being pushed.
   - After activation, the servo returns to its default position.

3. **Conveyor Synchronization**
   - The conveyor operates at a steady speed.
   - Timing adjustments were made through testing to ensure each cube reaches the correct servo before activation.

## Challenges & Solutions

1. **Power Supply Issues**
   - Initially used a 12V power supply for the conveyor, which was insufficient.
   - Switched to a 20V Lenovo laptop charger, solving the issue.

2. **Servo Overheating & Failure**
   - Accidentally supplied 15V instead of 5V, damaging all three servos.
   - Replaced them and added a stable 5V supply.

3. **Sensor Accuracy & Calibration**
   - External light interference caused inconsistent readings.
   - Kept cubes close to the sensor to reduce ambient light effects.
   - Adjusted code logic to filter out noise and improve accuracy.

4. **Failed Ultrasonic Sensor Experiment**
   - Attempted to use an ultrasonic sensor to detect cubes.
   - Did not function as expected, so the system now runs continuously.

5. **Burnt-Out Sensor**
   - Accidentally reversed polarity on a sensor, causing it to fry.
   - Replaced with a new one and double-checked wiring.
