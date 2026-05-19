# VANTIX - Smart Robot Car with Manual & Autonomous Modes


<p align="center">
  <img src="https://img.shields.io/badge/Platform-Arduino-blue?style=for-the-badge&logo=arduino" />
  <img src="https://img.shields.io/badge/Language-C%2B%2B%20%7C%20Dart-orange?style=for-the-badge" />
  <img src="https://img.shields.io/badge/App-Flutter-02569B?style=for-the-badge&logo=flutter" />
  <img src="https://img.shields.io/badge/Wireless-Bluetooth-informational?style=for-the-badge&logo=bluetooth" />
  <img src="https://img.shields.io/badge/Modes-7-success?style=for-the-badge" />
</p>

<br>

## 📋 Table of Contents

- [Project Overview](#project-overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [System Architecture](#system-architecture)
- [Bluetooth Command Protocol](#bluetooth-command-protocol)
- [Modes Overview](#modes-overview)
- [Operating Modes](#operating-modes)
- [Battery Monitoring](#battery-monitoring)
- [Flutter Mobile App](#flutter-mobile-app)
- [Bonus Features](#bonus-features)
- [Getting Started](#getting-started)
- [Project Structure](#project-structure)
- [Team](#team)



## 🔍 Project Overview

**VANTIX** is a three-wheel robotic car built on Arduino, supporting **7 operating modes** switchable at runtime over Bluetooth without restarting the controller. It is paired with a custom **Flutter mobile application** for wireless control and real-time telemetry.

Core modes: **Manual**, **Autonomous** obstacle avoidance, and **Auto-Parking**.  
Bonus modes: **Teach** (record a path), **Repeat** (replay it), **Leader**, and **Follower** for two-robot swarm coordination.



## ✨ Features

| Feature | Details |
|---|---|
| 🕹️ Manual Control | 8-direction movement, 9 speed levels, auto-stop timer |
| 🚗 Autonomous Navigation | 4-sensor obstacle avoidance FSM (front, back, left, right) |
| 🅿️ Auto-Parking | 6-state parking FSM with drift correction and gap detection |
| 🔋 Battery Monitoring | Voltage divider → ADC → percentage, sent every 3 s; LED warning |
| 🎓 Teach & Repeat | Record up to 200 movement + speed segments; millis()-based playback |
| 👥 Leader–Follower | Two robots coordinate via Bluetooth; follower uses gap control |
| 📱 Flutter App | Custom-built mobile app — controls, mode switching, battery gauge |



## 🔧 Hardware Requirements

See the bill of materials here

### Pin Map (from `config.h`)

| Signal | Arduino Pin |
|---|---|
| Motor Left — IN1 / IN2 / EN | 2 / 3 / 5 |
| Motor Right — IN3 / IN4 / EN | 4 / 7 / 6 |
| Ultrasonic Front TRIG / ECHO | 8 / 9 |
| Ultrasonic Back TRIG / ECHO | 10 / 11 |
| Ultrasonic Right TRIG / ECHO | 12 / 13 |
| Ultrasonic Left TRIG / ECHO | A1 / A2 |
| Battery ADC | A0 |
| Low-Battery LED | A5 |
| Bluetooth TX/RX | Hardware Serial (0 / 1) |

---

## 🏗️ System Architecture

```
┌──────────────────────────────────────────────────────┐
│                  Flutter Mobile App                  │
│   [ D-Pad ]  [ Speed ]  [ Mode ]  [ Battery Gauge ]  │
└─────────────────────┬────────────────────────────────┘
                      │ Bluetooth HC-05 @ 9600 baud
┌─────────────────────▼────────────────────────────────┐
│                VANTIX_second.ino                     │
│                                                      │
│  Bluetooth_ReadCommand()  ──►  Modes_HandleCommand() │
│                                        │             │
│                               Modes_Update()  (loop) │
│                                        │             │
│    ┌───────────────────────────────────┴──────────┐  │
│    │               Mode FSM                       │  │
│    │  Manual │ Autonomous │ Parking │ Teach       │  │
│    │  Repeat │ Leader     │ Follower              │  │
│    └───────────────────────────────────┬──────────┘  │
│                                        │             │
│              Motor / Sensor / Battery APIs           │
└────────────────────────────────────────┬─────────────┘
         │              │                │
    L298N Driver   HC-SR04 × 4      Voltage Divider
         │
   [Left Motor]  [Right Motor]
```


## 📡 Bluetooth Command Protocol

All commands are single ASCII characters sent from the app to the car at **9600 baud**.

### Mode-Switch Commands (valid in every mode)

| Char | Action |
|---|---|
| `U` | Switch to Manual Mode |
| `W` | Switch to Autonomous Mode |
| `X` | Switch to Auto-Parking Mode |
| `T` | Switch to Teach (Record) Mode |
| `P` | Switch to Repeat (Playback) Mode |
| `K` | Switch to Leader Mode |
| `Z` | Switch to Follower Mode |
| `S` | Emergency stop → Manual Mode |

### Movement Commands (Manual & Teach modes)

| Char | Movement |
|---|---|
| `F` | Forward |
| `B` | Backward |
| `L` | Pivot Left |
| `R` | Pivot Right |
| `G` | Forward-Left (arc) |
| `H` | Forward-Right (arc) |
| `I` | Backward-Left (arc) |
| `J` | Backward-Right (arc) |

### Speed Commands

| Char | `1` | `2` | `3` | `4` | `5` | `6` | `7` | `8` | `9` |
|---|---|---|---|---|---|---|---|---|---|
| PWM | 80 | 100 | 120 | 140 | **160** | 180 | 200 | 225 | 250 |

> Default speed is level `5` (PWM 160).

### Car → App Messages

| Message | Meaning |
|---|---|
| `"System Started"` | Boot confirmation |
| `"75%"` | Battery percentage (sent every 3 s) |
| `"MODE:LEADER_ACTIVE"` | Leader mode confirmed |
| `"MODE:FOLLOWER_ACTIVE"` | Follower mode confirmed |
| `"TEACH:OK"` | Recording started |
| `"TEACH:SAVED"` | Movement segment saved |
| `"TEACH:SPD=160"` | Speed segment saved |
| `"TEACH:FULL"` | Buffer full (200 segments) |
| `"REPEAT:OK"` | Playback started |
| `"REPEAT:DONE"` | Playback finished |
| `"REPEAT:EMPTY"` | No path recorded |
| `"LF:LEADER:READY"` | Leader initialised |
| `"LF:FOLLOWER:READY"` | Follower initialised |
| `"LF:LOST"` | Follower lost leader signal |
| `"LF:FOUND"` | Follower re-acquired leader |



## 🗂️ Modes Overview

VANTIX supports **7 operating modes**, all switchable at runtime via a single Bluetooth command — no restart required. Modes are managed by a central FSM in `Modes.cpp`.

| # | Mode | Command | Enum | Description |
|---|---|---|---|---|
| 1 | 🕹️ **Manual** | `U` | `MODE_MANUAL` | Full Bluetooth control with 8-direction movement and 9 speed levels |
| 2 | 🚗 **Autonomous** | `W` | `MODE_AUTONOMOUS` | Independent navigation using all 4 ultrasonic sensors to avoid obstacles |
| 3 | 🅿️ **Parking** | `X` | `MODE_PARKING` | Right-side auto-parking with gap detection and a timed 6-state maneuver sequence |
| 4 | 🎓 **Teach** | `T` | `MODE_TEACH` | Records every movement and speed command into a 200-segment SRAM buffer while the operator drives live |
| 5 | 🔁 **Repeat** | `P` | `MODE_REPEAT` | Replays the recorded path non-blocking using millis()-based timing; falls back to Manual on completion |
| 6 | 👑 **Leader** | `K` | `MODE_LEADER` | Broadcasts movement telemetry packets every 100 ms over Bluetooth to a paired Follower robot |
| 7 | 🤝 **Follower** | `Z` | `MODE_FOLLOWER` | Receives Leader packets, applies gap control to maintain a 25 cm following distance, and mirrors movement |

> **Emergency stop:** sending `S` from any mode immediately cuts motors and returns to Manual.



## 🎮 Operating Modes

### 1. 🕹️ Manual Mode

Standard Bluetooth-controlled driving. Each movement command starts a **600 ms auto-stop timer** — the car brakes automatically if no new command arrives, preventing runaway on connection loss.

```
Default speed:       speed[4] = 160 PWM
Auto-stop timeout:   600 ms
Turn ratio:          TurnRatio = 0.5  (arc turns use 50% inner-wheel speed)
```


### 2. 🚗 Autonomous Mode

Non-blocking FSM polled every loop iteration. All four sensors are read each cycle and compared against `OBSTACLE_DISTANCE = 25 cm`.

```
Obstacle threshold:  25 cm
Turn duration:       700 ms
Reverse duration:    600 ms
Cruise speed:        130 PWM
```

**State machine:**

```
         ┌──────────────────────────────────┐
         ▼                                  │ all clear
    AUTO_DRIVE ──── obstacle ──► pick turn direction
         │                              │
         │                   ┌──────────┴──────────┐
         │               left│                     │right
         │                   ▼                     ▼
         │          AUTO_AVOID_LEFT       AUTO_AVOID_RIGHT
         │                   │                     │
         │             700 ms│                     │700 ms
         │                   └──────────┬──────────┘
         │                              │
         │                    AUTO_AVOID_BACK  (both sides blocked)
         │                              │ 600 ms
         └──────────────────────────────┘
               all directions blocked → AUTO_STOPPED → wait
```



### 3. 🅿️ Auto-Parking Mode

Right-side perpendicular/parallel parking. The car moves forward along a wall with drift correction, detects a gap via a sudden increase in right-sensor distance, then executes a timed 6-state maneuver.

```
Park speed:                160 PWM
Solid-wall threshold:      32 cm
Rear safety stop:          8 cm
Gap detection delta:       22 cm
Overshoot time:            1900 ms
Reverse-in (angled right): 1800 ms
Reverse straight:          800 ms
Straighten (left arc):     1550 ms
Drift pulse / interval:    45 ms / 1200 ms
```

**State machine:**

```
PARK_FIND_SPOT
  │  (drift-corrected forward; watch right sensor for sudden gap)
  ▼
PARK_OVERSHOOT          (continue forward 1900 ms past gap)
  ▼
PARK_REVERSE_IN         (backward-right arc up to 1800 ms or rear stop)
  ▼
PARK_REVERSE_STRAIGHT   (backward straight 800 ms)
  ▼
PARK_STRAIGHTEN         (backward-left arc up to 1550 ms or rear stop)
  ▼
PARK_DONE               (motors stopped)
```

---

## 🔋 Battery Monitoring

A voltage divider (R1 = 30 kΩ, R2 = 10 kΩ) scales the 12 V battery into the Arduino ADC range.

```cpp
pinVolt     = (analogRead(A0) * 5.0) / 1023.0
batteryVolt = pinVolt * ((R1 + R2) / R2)         // ×4 scale factor
percentage  = (batteryVolt / 12.0) * 100          // clamped to [0, 100]
```

- Percentage is transmitted as `"75%"` every **3 seconds**.
- The **LED on pin A5** turns on when `batteryVolt ≤ 8.0 V` (low-battery warning).
- The Flutter app displays this as a gauge and can trigger an in-app alert.



## 📱 Flutter Mobile App

Built from scratch in **Flutter (Dart)**. Communicates with the robot over Bluetooth Classic (HC-05).

### Screens

| Screen | Description |
|---|---|
| Connect | Scan, pair, and connect to the robot's Bluetooth module |
| Control Panel | 8-direction D-pad, speed slider (1–9), mode selector buttons |
| Dashboard | Live battery gauge, current mode indicator, status log |
| Settings | Default speed, connection preferences |

### Suggested Packages

```yaml
dependencies:
  flutter_bluetooth_serial: ^0.4.0
  provider: ^6.0.0
  fl_chart: ^0.65.0
```



## 🌟 Bonus Features

### 🎓 Teach-and-Repeat

#### Teach Phase — `MODE_TEACH` (command `T`)

While the operator drives normally, the firmware timestamps every command transition and stores it as a `TeachSegment` in SRAM:

```cpp
typedef struct {
  char     Command;   // F/B/L/R/G/H/I/J/S  — or 'V' for a speed change
  uint16_t Value;     // movement: duration ms  |  speed: PWM value
} TeachSegment;

TeachSegment TeachPath[200];   // 200 segments × 3 bytes = 600 bytes
```

Speed changes (`1`–`9`) are recorded as `'V'` segments so the exact speed profile is replayed faithfully. The buffer holds up to **200 segments**; the app receives `"TEACH:FULL"` when it fills. Entering any other mode calls `Teach_Finalise()` to flush the last in-progress command.

#### Repeat Phase — `MODE_REPEAT` (command `P`)

Playback is **fully non-blocking** — no `delay()` calls anywhere.

```
Repeat_Init()    → validates buffer, loads first segment, sends "REPEAT:OK"
Repeat_Update()  → called every loop()
                   speed segments ('V') are applied instantly with no wait
                   movement segments run for their recorded duration
                   returns false when complete → automatically falls back to Manual
```

Playback speed: `REPEAT_SPEED = 140 PWM` (overridden if a `'V'` segment is first).

---

### 👥 Leader–Follower

Both robots run the **same firmware**. The role is selected at compile time in `LeaderFollowerConfig.h`:

```cpp
#define VEHICLE_ROLE  ROLE_LEADER    // change to ROLE_FOLLOWER for the second car
```

#### Leader — `MODE_LEADER` (command `K`)

Controlled from the app exactly like Manual mode. Every **100 ms** it broadcasts a telemetry packet over Bluetooth Serial to the follower:

```
Format:  LF:<cmd>,<speed>,<front>,<back>,<right>,<left>\n
Example: LF:F,160,42,180,95,110\n
```

#### Follower — `MODE_FOLLOWER` (command `Z`)

Reads the serial stream byte-by-byte into a 48-byte ring buffer, parses `LF:` packets, then applies **gap control** before executing any movement:

```
dist ≤ 12 cm (LF_TOO_CLOSE_CM)          →  emergency brake
dist < 20 cm (target − dead-band)        →  reverse away from leader
dist 20–30 cm (±5 cm dead-band)          →  hold position
dist > 30 cm (target + dead-band)        →  mirror leader's command
```

A **watchdog timer** stops the follower and sends `"LF:LOST"` if no valid packet arrives within **800 ms**.

#### Configuration Constants (`LeaderFollowerConfig.h`)

```cpp
#define LF_BROADCAST_INTERVAL_MS   100   // Leader packet rate (ms)
#define LF_TARGET_CM                25   // Ideal following distance (cm)
#define LF_DEADBAND_CM               5   // Tolerance band (±cm)
#define LF_TOO_CLOSE_CM             12   // Emergency stop distance (cm)
#define LF_LOST_TIMEOUT_MS         800   // Signal-lost watchdog (ms)
#define LF_LEADER_SPEED            160   // Leader default PWM
#define LF_FOLLOWER_SPEED          150   // Follower default PWM
```



## 🚀 Getting Started

### Prerequisites
- Arduino IDE ≥ 2.0
- Flutter SDK ≥ 3.0
- Android device with Bluetooth

### Firmware Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/vantix.git
   cd vantix/firmware
   ```

2. Open `VANTIX_second.ino` in Arduino IDE.

3. Verify all pin assignments in `config.h` match your physical wiring.

4. Set the robot's role in `LeaderFollowerConfig.h`:
   ```cpp
   // Car 1 (Leader):
   #define VEHICLE_ROLE  ROLE_LEADER

   // Car 2 (Follower):
   #define VEHICLE_ROLE  ROLE_FOLLOWER
   ```

5. Select your board (**Arduino Uno / Nano**), select the correct COM port, and upload.

6. Pair the HC-05 module with your phone. Default PIN: `1234`.

### Flutter App Setup

```bash
cd vantix/flutter_app
flutter pub get
flutter run
```



## 📁 Project Structure

```
vantix/
│
├── firmware/
│   ├── VANTIX_second.ino            # Entry point: setup() + loop()
│   ├── config.h                     # Pin definitions & all tuning constants
│   │
│   ├── Motor.h / Motor.cpp          # L298N driver — 8 directions + PWM speed
│   ├── Sensor.h / Sensor.cpp        # HC-SR04 × 4 (front / back / left / right)
│   ├── Bluetooth.h / Bluetooth.cpp  # Serial read/write wrapper
│   ├── Battery.h / Battery.cpp      # ADC → voltage → %, timed TX, LED control
│   │
│   ├── Modes.h / Modes.cpp          # 7-mode FSM, command router, update dispatcher
│   │
│   ├── TeachAndRepeat.h / TeachAndRepeat.cpp  # Teach & Repeat implementation
│   ├── LeaderFollower.h / LeaderFollower.cpp  # Leader & Follower implementation
│   └── LeaderFollowerConfig.h                # Role selection & LF constants
│
├── flutter_app/
│   ├── lib/
│   │   ├── main.dart
│   │   ├── screens/
│   │   │   ├── connect_screen.dart
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
│   └── circuit_diagram.pdf
│
└── README.md
```



## 👥 Team

| Name | Role |
|---|---|
| Member 1 | Firmware — Autonomous & Parking Modes |
| Member 2 | Firmware — Manual Mode & Bluetooth |
| Member 3 | Flutter App Development |
| Member 4 | Hardware Assembly & Power System |
| Member 5 | Bonus: Teach-and-Repeat & Leader–Follower |

> *Replace with your actual team member names and roles.*



<p align="center">VANTIX — Principles Of Microprocessors Project</p>
