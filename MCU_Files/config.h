#ifndef CONFIG_H
#define CONFIG_H

// ── Motor pins ────────────────────────────────────────────────────────────────
#define IN1        2
#define IN2        3
#define LeftSpeed  5

#define IN3        4
#define IN4        7
#define RightSpeed 6

#define MinPWM     0
#define MaxPWM     255
#define TurnRatio  0.5

// ── Bluetooth ─────────────────────────────────────────────────────────────────
#define BT_BAUD_RATE 9600

// ── Battery ───────────────────────────────────────────────────────────────────
#define BatteryPin  A0
#define R1          30000.0
#define R2          10000.0
#define BatteryFull 12.0

// ── Ultrasonic sensors ────────────────────────────────────────────────────────
#define FRONT_TRIG 8
#define FRONT_ECHO 9
#define BACK_TRIG  10
#define BACK_ECHO  11
#define RIGHT_TRIG 12
#define RIGHT_ECHO 13
#define LEFT_TRIG  A1
#define LEFT_ECHO  A2

#define ULTRASONIC_TIMEOUT 30000   // microseconds
// ── Teach-and-Repeat mode (Bonus 4.2) ───────────────────────────────────────── //✅add
#define TEACH_MAX_SEGMENTS      200   // Maximum recorded segments (300 bytes) //✅add //100
#define TEACH_MIN_DURATION_MS   50    // Minimum hold time to record (ms)      //✅add
#define REPEAT_SPEED            140   // Playback speed (PWM)                   //✅add
//#define REPEAT_OBSTACLE_CM      25    // Stop playback if obstacle closer (cm) //✅add//❌❌deleted
#define SUDDEN_GAP_THRESHOLD    20    // Gap detection threshold (cm)           //✅add

// ── Autonomous mode ───────────────────────────────────────────────────────────
#define OBSTACLE_DISTANCE      25  // cm

// ── Parking mode ──────────────────────────────────────────────────────────────
#define PARK_SPEED             160   // PWM - سرعة مناسبة للركن

#define PARK_BOUNDARY_DISTANCE 32    // cm - مسافة تعتبر "جسم صلب"
#define PARK_SAFE_DISTANCE     8     // cm - مسافة أمان خلفية (وقف فوري)

#define PARK_OVERSHOOT_MS      1900  // ms - أهم قيمة (يتقدم بعد اكتشاف الفجوة)
#define PARK_REVERSE_IN_MS     1800  // ms - وقت الرجوع مع الدوران يمين
// ✅add: ثابت جديد لحركة الرجوع للخلف مباشرة (بدون انحراف)
#define PARK_REVERSE_STRAIGHT_MS 800  // ms - وقت الرجوع للخلف مستقيم
#define PARK_STRAIGHTEN_MS     1550  // ms - وقت التقويم (دوران يسار)

#define DRIFT_INTERVAL_MS      1200  //850
#define DRIFT_PULSE_MS         45

// لكشف الفجوة
#define SUDDEN_GAP_THRESHOLD   22    // الفرق في المسافة الذي يعتبر فجوة

#endif