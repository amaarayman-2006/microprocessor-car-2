# 🤖 Smart Robot Car — Manual, Autonomous & Auto-Parking Modes

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino-blue?style=for-the-badge&logo=arduino" />
  <img src="https://img.shields.io/badge/Language-C%2B%2B%20%7C%20Dart-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/App-Flutter-02569B?style=for-the-badge&logo=flutter" />
  <img src="https://img.shields.io/badge/Wireless-Bluetooth-informational?style=for-the-badge&logo=bluetooth" />
  <img src="https://img.shields.io/badge/Status-Complete-success?style=for-the-badge" />
</p>

<br>

## 📋 Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [System Architecture](#system-architecture)
- [Operating Modes](#operating-modes)
- [Battery Monitoring](#battery-monitoring)
- [Flutter Mobile App](#flutter-mobile-app)
- [Bonus Features](#bonus-features)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Team](#team)

---

## 🔍 Project Overview

The **Smart Robot Car** is a four-wheel robotic platform capable of operating in three distinct modes: **Manual** (Bluetooth-controlled), **Autonomous** (sensor-based obstacle avoidance), and **Auto-Parking** (automated slot detection and alignment). The system is paired with a custom-built **Flutter mobile application** for full wireless control and real-time telemetry.

As an extension, the project also implements two bonus features: **Leader–Follower swarm coordination** and **Teach-and-Repeat path recording & playback**.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🕹️ Manual Control | Drive the car via Bluetooth using a custom Flutter app |
| 🚗 Autonomous Navigation | Ultrasonic-based obstacle detection and avoidance |
| 🅿️ Auto-Parking | Detects a parking slot and executes parallel or perpendicular parking |
| 🔋 Battery Monitoring | Real-time voltage sensing with live percentage display in the app |
| 👥 Leader–Follower | Two robots coordinate — one leads, one follows wirelessly |
| 🔁 Teach & Repeat | Record a manual path, then replay it autonomously |
| 📱 Flutter App | Custom-built mobile app with controls, mode switching, and gauges |

---

## 🔧 Hardware Requirements

### Mechanical
- Four-wheel robotic chassis
- 2× DC motors (rear/front drive wheels)
- 2× Caster/free wheels

### Electronics
| Component | Specification |
|---|---|
| Microcontroller | Arduino Uno / Nano |
| Motor Driver | L298N, L293D, or equivalent |
| Ultrasonic Sensors | HC-SR04 (front, left, right) |
| Bluetooth Module | HC-05 / HC-06 / BLE |
| Battery | Li-ion / Lead-acid / AA pack |
| Voltage Divider | For battery monitoring (analog pin) |
| Power Switch | For safe system ON/OFF |

### Wiring Notes
- Motors powered through the motor driver from the main battery
- Microcontroller powered via onboard regulator or a separate 5V line
- Voltage divider scales battery voltage to ADC-safe range (0–5V or 0–3.3V)
- All grounds must be common across subsystems

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────┐
│                   Flutter Mobile App                │
│    [ Controls ]  [ Mode Select ]  [ Battery Gauge ] │
└──────────────────────┬──────────────────────────────┘
                       │ Bluetooth (HC-05 / BLE)
┌──────────────────────▼──────────────────────────────┐
│                  Microcontroller                    │
│  ┌────────────┐  ┌──────────────┐  ┌─────────────┐ │
│  │ Mode FSM   │  │ Sensor Layer │  │ Battery Mon.│ │
│  └─────┬──────┘  └──────┬───────┘  └──────┬──────┘ │
│        │                │                 │         │
│  ┌─────▼────────────────▼─────────────────▼──────┐ │
│  │              Motor Control (PWM)               │ │
│  └────────────────────┬───────────────────────────┘ │
└───────────────────────┼─────────────────────────────┘
                        │
              ┌─────────▼─────────┐
              │   L298N / L293D   │
              │  Motor Driver IC  │
              └────┬──────────┬───┘
                   │          │
              [Motor L]   [Motor R]
```

---

## 🎮 Operating Modes

### 1. 🕹️ Manual Mode
The car is fully controlled by the Flutter app over Bluetooth.

**Supported commands:**

| Command | Description |
|---|---|
| `F` | Move Forward |
| `B` | Move Backward |
| `L` | Turn Left |
| `R` | Turn Right |
| `S` | Stop |
| `0`–`9` | Speed level (PWM duty cycle) |
| `M` | Switch to Manual Mode |
| `A` | Switch to Autonomous Mode |
| `P` | Switch to Auto-Parking Mode |

---

### 2. 🚗 Autonomous Mode
The car navigates independently using ultrasonic sensors.

**Sensor Configuration:** 3-sensor array (front, left, right)

**Logic flow:**
1. Continuously measure distances from all three sensors
2. If front is clear → move forward
3. If obstacle detected in front → compare left and right readings
4. Turn toward the side with more clearance
5. Resume forward motion

**Threshold constants:**
```cpp
#define OBSTACLE_DIST_CM     30
#define SAFE_DIST_CM         50
#define TURN_DURATION_MS    400
```

---

### 3. 🅿️ Auto-Parking Mode
The robot detects a valid parking slot and executes alignment maneuvers.

**Detection approach:**
- Side-mounted ultrasonic sensor scans for a gap large enough to park
- Once a slot is identified, the car stops, aligns, then executes parking sequence

**Supported parking types:**
- Perpendicular parking (right-angle slot)
- Parallel parking (roadside slot)

**State machine:**
```
SCAN → SLOT_DETECTED → ALIGN → PARK_ENTER → PARK_CORRECT → PARKED
```

---

## 🔋 Battery Monitoring

A voltage divider circuit scales the battery voltage to within the ADC input range. The firmware reads the analog voltage, maps it to a percentage, and transmits it to the app every few seconds.

```cpp
#define BATTERY_PIN         A0
#define BATTERY_FULL_V      8.4f    // 2S Li-ion fully charged
#define BATTERY_EMPTY_V     6.0f    // Cutoff voltage
#define LOW_BATTERY_PCT     20      // Trigger warning below this
#define BATTERY_SEND_MS     3000    // Report interval
```

**App behavior:**
- Displays live battery percentage as a gauge/progress bar
- Shows color-coded warnings (green → yellow → red)
- Triggers a low-battery alert when below threshold

---

## 📱 Flutter Mobile App

The mobile app is built from scratch using **Flutter (Dart)** and communicates with the robot via Bluetooth Classic or BLE.

### Screens

| Screen | Description |
|---|---|
| **Home / Connect** | Scan and pair with the robot's Bluetooth module |
| **Control Panel** | D-pad joystick, speed slider, mode buttons |
| **Dashboard** | Battery gauge, current mode indicator, live status |
| **Settings** | Configure speed defaults, connection preferences |

### Key Packages
```yaml
dependencies:
  flutter_bluetooth_serial: ^0.4.0   # or flutter_blue_plus for BLE
  provider: ^6.0.0
  fl_chart: ^0.65.0                  # Battery gauge chart
```

### Command Protocol
Commands are single ASCII characters or short strings terminated with `\n`:
```
"F\n"  → Forward
"B\n"  → Backward
"L\n"  → Left
"R\n"  → Right
"S\n"  → Stop
"5\n"  → Speed level 5
"A\n"  → Autonomous mode
"P\n"  → Auto-parking mode
```

Battery data is sent **from the robot to the app** as:
```
"BAT:75\n"  → 75% battery remaining
```

---

## 🌟 Bonus Features

### 👥 Leader–Follower (Swarm Mode)

Two robots communicate wirelessly (ESP-NOW or Bluetooth mesh). The **leader** robot operates in any mode while broadcasting its movement commands. The **follower** robot receives and mirrors those commands with a configurable delay.

**Communication protocol (ESP-NOW example):**
```cpp
typedef struct {
  char  command;      // 'F', 'B', 'L', 'R', 'S'
  uint8_t speed;      // 0–255 PWM
  uint8_t mode;       // 0=Manual, 1=Auto, 2=Park
} RobotPacket;
```

**Behaviors demonstrated:**
- Synchronized forward/backward motion
- Formation maintenance during turns
- Follower collision avoidance (follower keeps independent obstacle detection active)

---

### 🔁 Teach-and-Repeat

#### Teach Phase (Record)
While driving manually via Bluetooth, the microcontroller records timestamped commands into **EEPROM / flash (non-volatile memory)**.

```cpp
typedef struct {
  char     command;       // Movement command
  uint16_t duration_ms;   // How long to execute it
} PathEntry;

#define MAX_PATH_ENTRIES  100
```

- Recording begins on a dedicated Bluetooth command (e.g., `"REC_START\n"`)
- Recording ends on `"REC_STOP\n"`
- Path is persisted to EEPROM — survives power cycles

#### Repeat Phase (Playback)
On `"PLAYBACK\n"` command, the robot replays each recorded entry in sequence.

- Ultrasonic sensors remain active during playback for safety
- If an obstacle is detected mid-playback, the robot pauses and waits for clearance
- Playback completes when all entries are exhausted or a stop command is received

---

## 🚀 Getting Started

### Prerequisites
- Arduino IDE (≥ 2.0) or PlatformIO
- Flutter SDK (≥ 3.0)
- Android device with Bluetooth

### Firmware Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/smart-robot-car.git
   cd smart-robot-car/firmware
   ```

2. Open `smart_robot_car.ino` in Arduino IDE.

3. Select your board (Arduino Uno / Nano) and the correct COM port.

4. Review and adjust pin definitions in `config.h`:
   ```cpp
   #define MOTOR_LEFT_EN   5
   #define MOTOR_LEFT_IN1  6
   #define MOTOR_LEFT_IN2  7
   #define MOTOR_RIGHT_EN  10
   #define MOTOR_RIGHT_IN1 8
   #define MOTOR_RIGHT_IN2 9
   #define TRIG_FRONT      A1
   #define ECHO_FRONT      A2
   // ... etc.
   ```

5. Upload to the microcontroller.

### Flutter App Setup

1. Navigate to the app directory:
   ```bash
   cd smart-robot-car/flutter_app
   flutter pub get
   ```

2. Connect your Android device and run:
   ```bash
   flutter run
   ```

3. Pair with the robot's Bluetooth module (default PIN: `1234`).

---

## 📁 Project Structure

```
smart-robot-car/
│
├── firmware/
│   ├── smart_robot_car.ino      # Main entry point
│   ├── config.h                 # Pin definitions & constants
│   ├── motor_control.h/.cpp     # Motor driver abstraction
│   ├── ultrasonic.h/.cpp        # Sensor reading functions
│   ├── bluetooth_comm.h/.cpp    # BT command parsing & TX
│   ├── battery_monitor.h/.cpp   # ADC reading & percentage calc
│   ├── autonomous.h/.cpp        # Obstacle avoidance logic
│   ├── auto_parking.h/.cpp      # Parking state machine
│   ├── teach_repeat.h/.cpp      # Path recording & playback
│   └── swarm.h/.cpp             # Leader–follower logic
│
├── flutter_app/
│   ├── lib/
│   │   ├── main.dart
│   │   ├── screens/
│   │   │   ├── home_screen.dart
│   │   │   ├── control_screen.dart
│   │   │   └── settings_screen.dart
│   │   ├── widgets/
│   │   │   ├── dpad_widget.dart
│   │   │   ├── battery_gauge.dart
│   │   │   └── mode_selector.dart
│   │   └── services/
│   │       └── bluetooth_service.dart
│   └── pubspec.yaml
│
├── docs/
│   ├── circuit_diagram.pdf
│   └── demo_video_link.md
│
└── README.md
```

---

## 👥 Team

| Name | Role |
|---|---|
| Member 1 | Firmware — Autonomous & Parking Modes |
| Member 2 | Firmware — Manual Mode & BT Communication |
| Member 3 | Flutter App Development |
| Member 4 | Hardware Assembly & Power System |
| Member 5 | Bonus: Swarm & Teach-and-Repeat |

> *Replace with your actual team member names and roles.*

---

<p align="center">Made with ❤️ — Microprocessors Systems Project</p>
