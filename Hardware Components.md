# Hardware Bill of Materials
This document outlines the hardware components required to build and operate the VANTIX smart robot car.
## Core Processing & Control
The system is built around the ATmega328p architecture, utilizing a Bluetooth module for wireless communication and control from a mobile application.
| Component | Qty | Description / Specifications |
| :--- | :---: | :--- |
| **Arduino Uno** | 1 | Main component of the project. Handles logic, processing, ADC readings, PWM motor control, and ultrasonic sensor timing. |
| **HC-05 Bluetooth Module** | 2 | Wireless serial communication at 9600 baud. One per robot. Used for app-to-robot control and robot-to-robot Leader–Follower data exchange. |
## Actuation & Drive System
The drivetrain relies on a dual H-bridge setup to independently control the speed and direction of the two DC motors, enabling differential drive and 8-direction movement.
| Component | Qty | Description / Specifications |
| :--- | :---: | :--- |
| **L298N Motor Driver** | 1 | Dual H-bridge module for driving high-current loads via microcontroller logic. Supports PWM speed control and forward/reverse direction on each channel. |
| **DC Geared Motor with Wheel** | 2 | Rear drive motors providing differential drive to the chassis. Speed controlled via PWM (0–255). |
| **Caster / Free Wheel** | 2 | Passive support wheels mounted on the remaining two corners of the chassis. |
| **Robot Chassis** | 1 | Base mechanical frame for mounting motors, electronics, and power supply. |
## Sensing
Four ultrasonic sensors provide full 360° proximity awareness for obstacle avoidance, auto-parking, and Leader–Follower gap control.
| Component | Qty | Description / Specifications |
| :--- | :---: | :--- |
| **HC-SR04 Ultrasonic Sensor** | 4 | One each facing front, back, left, and right. Triggered with a 10 µs pulse; echo duration converted to distance (cm). Timeout set to 30,000 µs. |
## Power Management
A series-connected lithium-ion pack provides sufficient voltage to power the L298N driver and motors, while the Arduino regulates its own 5V logic supply.
| Component | Qty | Description / Specifications |
| :--- | :---: | :--- |
| **Li-ion / Lead-Acid Battery Pack** | 1 | Provides 12V nominal (e.g. 3S Li-ion). Powers motors via L298N and Arduino via onboard regulator. |
| **Voltage Divider (R1 = 30kΩ, R2 = 10kΩ)** | 1 | Scales battery voltage (up to ~16V) down to ADC-safe range (0–5V) on pin A0 for real-time battery monitoring. |
| **Power Switch** | 1 | Master SPST switch to isolate the battery pack from the active circuit. |
## Prototyping & Connectivity
Standard prototyping components for status indication and signal routing.
| Component | Qty | Description / Specifications |
| :--- | :---: | :--- |
| **Breadboard** | 1 | Half-size board for routing low-voltage logic and indicator circuits. |
| **LEDs** | 2 | Visual status indicators ( Green for POWER ON, Red is when battery voltage is below 8V ). |
| **Resistors** | Set | Used for LED current-limiting (typically 220Ω – 330Ω) and potential pull-up/down logic. |
| **Male-to-Male Jumpers** | Set | Used for routing signals across the breadboard and Arduino. |
| **Female-to-Male Jumpers** | Set | Used for connecting sensors and modules (ultrasonic, Bluetooth) to the Arduino. |
| **Crocodile Jumpers** | Set | Used for temporary, secure connections to battery terminals and motor leads. |
