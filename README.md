# VL53L8CX Sensor Project

This project demonstrates how to use the VL53L8CX distance sensor with an Arduino platform. The code initializes three VL53L8CX sensors, reads their distance measurements, and controls motor speed based on the detected distance.

## Features

- Initializes three VL53L8CX sensors via I2C.
- Reads distance data and prints results to the Serial Monitor.
- Controls motor speed based on detected distance:
  - Near (0-10 mm): High speed
  - Medium (10-250 mm): Medium speed
  - Far (250-1000 mm): Low speed
  - No object detected: Stop

## Components Required

- Arduino board (compatible with the SAM DUE)
- VL53L8CX distance sensors (3 units)
- Motors (3 units)
- Motor driver or similar circuit for controlling motor speed
- Jumper wires
- Breadboard (optional)

## Wiring Diagram

1. **VL53L8CX Sensors:**
   - Connect each sensor to the I2C pins of the Arduino.
   - Use defined power and ground pins for enabling sensors.

2. **Motors:**
   - Connect motors to the specified power pins (`PWR_MOT_1`, `PWR_MOT_2`, `PWR_MOT_3`).
   - Ensure ground is connected.

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/yourusername/vl53l8cx-sensor-project.git
