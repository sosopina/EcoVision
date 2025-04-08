
# EcoVision Smart Factory: Integrated Robotic Arm & Sorting Station

## System Overview
This documentation describes the integration of two key components in the EcoVision Smart Factory:

- **Robotic Arm Station**: Handles pick-and-place operations.
- **Color Sorting Station**: Sorts cubes based on color.

The two stations are synchronized via serial communication between two Arduino Uno boards. The system ensures sequential operation:

1. Sorting Station detects a cube’s color.
2. Robotic Arm moves the cube through inspection and placement.
3. Sorting Station completes sorting after receiving confirmation from the Robotic Arm.

## Key Integration Components

### 1. Serial Communication (TX/RX)
**Purpose**: Coordinate timing between stations.

**Protocol**:
- **Signal S**: Sent from Sorting Station to Robotic Arm to trigger the pick-and-place sequence.
- **Signal D**: Sent from Robotic Arm to Sorting Station to resume sorting.
- **Baud Rate**: 9600.

**Wiring Guide**:

| Arduino 1 (Robotic Arm) | Arduino 2 (Sorting Station) |
|-------------------------|-----------------------------|
| TX (Pin 1)              | RX (Pin 0)                  |
| RX (Pin 0)              | TX (Pin 1)                  |
| GND                     | GND                         |

⚠️ **Critical Notes**:
- Disconnect TX/RX wires during code upload to avoid conflicts.
- Ensure both Arduinos share a common ground.

### 2. State Machine Logic

#### Robotic Arm (Arduino 1)
- **State 1**: Idle, waiting for S signal.
- **State 2**: Executes `initial_pickup()`, `ai_station()`, and `sorting_station()` sequences.
- **State 3**: Sends D signal upon completion.

#### Sorting Station (Arduino 2)
- **State 1: IDLE** – Continuously checks for colors.
- **State 2: WAIT_FOR_ARM** – Halts conveyor, waits for D signal.
- **State 3: CONVEYOR_ACTIVE** – Moves cube for color-specific duration.
- **State 4: SERVO_ACTIVE** – Pushes cube to correct lane.

## Full Wiring Diagram

### Robotic Arm Station
| Component         | Arduino Pin | Purpose                          |
|------------------|-------------|----------------------------------|
| Servo1 (Shoulder)| 3           | Controls vertical arm movement   |
| Servo2 (Elbow)   | 5           | Adjusts horizontal reach         |
| Servo3 (Gripper) | 6           | Opens/closes to grip cubes       |
| Servo4 (Base)    | 9           | Rotates arm between stations     |

### Sorting Station
| Component         | Arduino Pin | Purpose                          |
|------------------|-------------|----------------------------------|
| Servo1 (Red)     | 3           | Pushes red cubes                 |
| Servo2 (Green)   | 5           | Pushes green cubes               |
| Servo3 (White)   | 6           | Pushes white cubes               |
| Relay            | 7           | Controls conveyor belt power     |
| TCS34725 Sensor  | I2C (A4/A5)  | Reads cube colors                |

### Shared Components
**Power**:
- Use separate 5V supplies for servos to avoid Arduino overload.
- Conveyor belt powered by 20V Lenovo charger via relay.

## Workflow

### Startup:
- Both Arduinos initialize servos to **180°** and reset conveyors.

### Color Detection:
- Sorting Station detects cube color (Red/Green/White).

### Arm Activation:
- Sorting Station sends **S** to Robotic Arm.

### Robotic Arm Sequence:
- Picks up cube → AI inspection → Places on sorting conveyor.
- Sends **D** signal upon completion.

### Sorting Completion:
- Sorting Station runs conveyor for color-specific delay (14s/20s/5s).
- Activates servo to push cube into lane.

## Technical Considerations

### 1. Timing Synchronization
- **Conveyor Delays**: Adjusted to ensure cubes reach the correct servo position.
  - Red: 14s → Servo1
  - Green: 20s → Servo2
  - White: 5s → Servo3
- **Servo Hold Time**: 2 seconds (prevents partial pushes).

### 2. Error Handling
- **Sensor Failures**:
  - Sorting Station freezes if TCS34725 is not detected.
- **Serial Signals**:
  - Robotic Arm ignores invalid serial signals.

- **Power Stability**:
  - Servos powered externally (5V 2A) to avoid Arduino brownouts.

### 3. Calibration
- **Color Thresholds**: Adjust in code based on sensor readings (use Serial Monitor for debugging).
- **Servo Angles**: Tune `POS_PUSH` and `POS_IDLE` values for physical alignment.



For troubleshooting, monitor serial communication with the Arduino IDE’s Serial Monitor and verify servo power connections.
