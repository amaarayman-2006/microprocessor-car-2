#ifndef CONFIG_H
#define CONFIG_H

// ── Motor pins ────────────────────────────────────────────────────────────────
#define IN1 2
#define IN2 3
#define LeftSpeed 5
#define IN3 4
#define IN4 7
#define RightSpeed 6

#define MinPWM 0
#define MaxPWM 255
#define TurnRatio 0.6

// ── Bluetooth ─────────────────────────────────────────────────────────────────
#define BT_BAUD_RATE 9600

// ── Battery ───────────────────────────────────────────────────────────────────
#define BatteryPin A0
#define R1 30000.0
#define R2 10000.0
#define BatteryFull 12.0

// ── Ultrasonic sensors ────────────────────────────────────────────────────────
#define FRONT_TRIG 8
#define FRONT_ECHO 9
#define BACK_TRIG 10
#define BACK_ECHO 11
#define RIGHT_TRIG 12
#define RIGHT_ECHO 13
#define LEFT_TRIG A1
#define LEFT_ECHO A2

#define ULTRASONIC_TIMEOUT 30000

// ── Teach-and-Repeat mode ─────────────────────────────────────────────────────
#define TEACH_MAX_SEGMENTS 200
#define TEACH_MIN_DURATION_MS 50
#define REPEAT_SPEED 140

// ── Autonomous mode ───────────────────────────────────────────────────────────
#define AUTO_SPEED 140
#define OBSTACLE_DISTANCE 25

// ── Parking mode ──────────────────────────────────────────────────────────────
#define PARK_SPEED 140

#define PARK_RIGHT_CAR_DETECT_CM 26
#define PARK_SPOT_DETECT_CM 28
#define PARK_BACK_CAR_DETECT_CM 14

#define PARK_TURN_TIME_MS 600
#define PARK_STOP_DELAY_MS 200
#define PARK_TURN_TIME_MS2 450

#endif