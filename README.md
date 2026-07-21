# Flower Robot
An automated ATmega328P-based robotic flower which opens/closes its petals and displays messages depending on whether an individual is detected nearby.

## Overview & Features
* **Proximity Sensing:** Uses a sonar range finder to track user distance.
* **Flower Actuation:** Controls 4 servos via a 16-channel PCA9685 PWM driver over I2C to open/close petals with fishing lines.
* **State Machine:** Transitions between states for closing servos, opening servos, and powering off.
* **LCD Display:** Displays custom messages on LCD1602 display.

## Hardware Requirements
* **Microcontroller:** Arduino Nano (ATmega328P)
* **Distance Sensor:** HC-SR04 Ultrasonic Sensor
* **Display:** LCD1602 Module (Parallel 4-bit)
* **Actuators:** 4 x Servo Motors MG995
* **Servo Driver:** PCA9685 16-Channel 12-bit PWM Driver
* **Custom PCB:** Integrated circuit board for routing power and communication

## Repository Structure

```text
.
├── .vscode/
│   └── extensions.json  # Recommended VS Code extensions (PlatformIO, C/C++)
├── include/
│   └── config.h         # Pins, servo limits, etc
├── src/
│   └── main.cpp         # Main FSM logic and sensor polling
├── platformio.ini       # Build configuration, board definition, and libraries
└── README.md            # Documentation
```

## Getting Started
1. Install [VS Code](https://code.visualstudio.com/) and the **PlatformIO IDE** extension
2. Clone this repository
3. Build hardware using parts listed above
4. Connect Arduino Nano via USB-C
5. Use PlatformIO upload button (or press `Ctrl` + `Alt` + `U`) to compile and flash to hardware

## Configuration & Calibration
All user-configurable parameters are located in `include/config.h`.
* `SERVO_MIN/SERVO_MAX`: Sets boundaries for servos which control how much the petals open.
* `RANGE`: Controls how far the user can be (in cm) for the flower to still open.