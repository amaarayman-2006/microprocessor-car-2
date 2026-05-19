#include <Arduino.h>
#include "Modes.h"
#include "Config.h"
#include "Motor.h"
#include "Sensor.h"
#include "TeachAndRepeat.h"  //✅add
#include "LeaderFollower.h"   //✅add (جديد)
#include "Bluetooth.h"       //✅add (هذا السطر سيحل المشكلة)

static int speed[] = {80, 100, 120, 140, 160, 180, 200, 225, 250};
static int AutoSpeed = 130;

static RobotMode CurrentMode = MODE_MANUAL;

// ═══════════════════════════════════════════════════════════════════════════════
// MANUAL MODE TIMERS (Press & Release)
// ═══════════════════════════════════════════════════════════════════════════════
static unsigned long ManualMoveStartTime = 0;
static bool ManualIsMoving = false;
static const int ManualMoveDuration = 600;  

// ═══════════════════════════════════════════════════════════════════════════════
// AUTONOMOUS
// ═══════════════════════════════════════════════════════════════════════════════
typedef enum
{
  AUTO_DRIVE,
  AUTO_AVOID_LEFT,
  AUTO_AVOID_RIGHT,
  AUTO_AVOID_BACK,
  AUTO_STOPPED
} AutoState;

static AutoState     CurrentAutoState   = AUTO_DRIVE;
static unsigned long AutoStateStartTime = 0;

static const int AvoidTurnTime = 700;
static const int AvoidBackTime = 600;

// ═══════════════════════════════════════════════════════════════════════════════
// PARKING MODE
// ═══════════════════════════════════════════════════════════════════════════════
typedef enum
{
  PARK_IDLE = 0,
  PARK_FIND_SPOT,
  PARK_OVERSHOOT,
  PARK_REVERSE_IN,
  PARK_REVERSE_STRAIGHT,      // ✅add: حركة الرجوع للخلف مباشرة (بدون انحراف)
  PARK_STRAIGHTEN,
  PARK_DONE
} ParkingState;

static ParkingState  CurrentParkingState = PARK_IDLE;
static unsigned long ParkingPhaseStart   = 0;

static bool ParkSeenSolidFirst = false;
static long lastRightDistance  = 0;

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

static void SetAutoState(AutoState s)
{
  CurrentAutoState   = s;
  AutoStateStartTime = millis();
}

static void SetParkingState(ParkingState s)
{
  CurrentParkingState = s;
  ParkingPhaseStart   = millis();
}

static bool DirectionIsBlocked(long d)
{
  return (d != -1 && d <= OBSTACLE_DISTANCE);
}

static bool DirectionIsOpen(long d)
{
  return (d == -1 || d > OBSTACLE_DISTANCE);
}

static bool RightIsSolid(void)
{
  long r = Sensor_GetRightDistance();
  return (r != -1 && r <= PARK_BOUNDARY_DISTANCE);
}

static bool BackTooClose(void)
{
  long b = Sensor_GetBackDistance();
  return (b != -1 && b <= PARK_SAFE_DISTANCE);
}

static bool IsSuddenGap(long currentDist)  //✅add
{                                          //✅add
  if (lastRightDistance == 0) return false;//✅add
  return (currentDist > lastRightDistance + SUDDEN_GAP_THRESHOLD); //✅add
}                                          //✅add

// Drift Correction
static void DriftCorrectedForward(void)
{
  unsigned long now = millis();
  static unsigned long DriftLastNudgeTime = 0;
  static bool DriftInPulse = false;

  if (DriftInPulse)
  {
    Motor_Right();
    if (now - DriftLastNudgeTime >= DRIFT_PULSE_MS)
      DriftInPulse = false;
  }
  else
  {
    Motor_Forward();
    if (now - DriftLastNudgeTime >= DRIFT_INTERVAL_MS)
    {
      DriftLastNudgeTime = now;
      DriftInPulse = true;
    }
  }
}

// Execute a movement-character command  //✅add
static void ExecuteMovement(char cmd)     //✅add
{                                         //✅add
  switch (cmd)                            //✅add
  {                                       //✅add
    case 'F': Motor_Forward();       break; //✅add
    case 'B': Motor_Backward();      break; //✅add
    case 'L': Motor_Left();          break; //✅add
    case 'R': Motor_Right();         break; //✅add
    case 'G': Motor_ForwardLeft();   break; //✅add
    case 'H': Motor_ForwardRight();  break; //✅add
    case 'I': Motor_BackwardLeft();  break; //✅add
    case 'J': Motor_BackwardRight(); break; //✅add
    default:                         break; //✅add
  }                                       //✅add
}                                         //✅add

// ═══════════════════════════════════════════════════════════════════════════════
// BASIC FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════
RobotMode Modes_GetCurrentMode(void) { return CurrentMode; }

void Modes_Init(void)
{
  CurrentMode         = MODE_MANUAL;
  CurrentAutoState    = AUTO_DRIVE;
  CurrentParkingState = PARK_IDLE;
  ParkSeenSolidFirst  = false;
  lastRightDistance   = 0;

  Motor_SetSpeed(speed[4]);
  Motor_Stop();
}

void Modes_SetMode(RobotMode Mode)
{
  Motor_Stop();

  // ── Teardown: flush any in-progress teach recording ─────────────────────── //✅add
  if (CurrentMode == MODE_TEACH)                                               //✅add
  {                                                                            //✅add
    Teach_Finalise();                                                          //✅add
  }                                                                            //✅add

  CurrentMode = Mode;

  if (CurrentMode == MODE_MANUAL)
  {
    Motor_SetSpeed(speed[4]);
  }
  else if (CurrentMode == MODE_AUTONOMOUS)
  {
    Motor_SetSpeed(AutoSpeed);
    SetAutoState(AUTO_DRIVE);
  }
  else if (CurrentMode == MODE_PARKING)
  {
    Motor_SetSpeed(PARK_SPEED);
    ParkSeenSolidFirst = RightIsSolid();
    lastRightDistance  = 0;
    SetParkingState(PARK_FIND_SPOT);
  }
  // ── New modes from TeachAndRepeat ──────────────────────────────────────── //✅add
  else if (CurrentMode == MODE_TEACH)                                          //✅add
  {                                                                            //✅add
    Motor_SetSpeed(speed[4]);                                                  //✅add
    Teach_Init();                                                              //✅add
  }                                                                            //✅add
  else if (CurrentMode == MODE_REPEAT)                                         //✅add
  {                                                                            //✅add
    if (!Repeat_Init())                                                        //✅add
    {                                                                          //✅add
      CurrentMode = MODE_MANUAL;                                               //✅add
      Motor_SetSpeed(speed[4]);                                                //✅add
    }                                                                          //✅add
  }                                                                            //✅add
  // ── New modes from LeaderFollower ───────────────────────────────────────── //✅add
  else if (CurrentMode == MODE_LEADER)                                         //✅add
  {                                                                            //✅add
    LF_Leader_Init();                                                          //✅add
    Bluetooth_Send("MODE:LEADER_ACTIVE");                                      //✅add
  }                                                                            //✅add
  else if (CurrentMode == MODE_FOLLOWER)                                       //✅add
  {                                                                            //✅add
    LF_Follower_Init();                                                        //✅add
    Bluetooth_Send("MODE:FOLLOWER_ACTIVE");                                    //✅add
  }                                                                            //✅add
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMMAND HANDLER
// ═══════════════════════════════════════════════════════════════════════════════
void Modes_HandleCommand(char Command)
{
  // ── Global mode-switch commands (valid in every mode) ───────────────────── //✅add
  switch (Command)                                                             //✅add
  {                                                                            //✅add
    case 'U': Modes_SetMode(MODE_MANUAL);     return;                          //✅add
    case 'W': Modes_SetMode(MODE_AUTONOMOUS); return;                          //✅add
    case 'X': Modes_SetMode(MODE_PARKING);    return;                          //✅add
    case 'T': Modes_SetMode(MODE_TEACH);      return;                          //✅add
    case 'P': Modes_SetMode(MODE_REPEAT);     return;                          //✅add
    case 'K': Modes_SetMode(MODE_LEADER);     return;                          //✅add (جديد)
    case 'Z': Modes_SetMode(MODE_FOLLOWER);   return;                          //✅add (جديد)
  }                                                                            //✅add

  switch (Command)
  {
    case 'S':
      if (CurrentMode == MODE_TEACH) Teach_Finalise();  //✅add
      Motor_Stop();
      CurrentMode         = MODE_MANUAL;
      CurrentAutoState    = AUTO_DRIVE;
      CurrentParkingState = PARK_IDLE;
      ManualIsMoving      = false;
      return;
  }

  // ── Mode-specific command handling ──────────────────────────────────────── //✅add
  switch (CurrentMode)                                                          //✅add
  {                                                                             //✅add
    case MODE_MANUAL:                                                           //✅add
      if (Command >= '1' && Command <= '9')                                     //✅add
      {                                                                         //✅add
        Motor_SetSpeed(speed[Command - '1']);                                   //✅add
        return;                                                                 //✅add
      }                                                                         //✅add
      // Manual Movement with auto stop                                         //✅add
      switch (Command)                                                          //✅add
      {                                                                         //✅add
        case 'F': case 'B': case 'L': case 'R':                                 //✅add
        case 'G': case 'H': case 'I': case 'J':                                 //✅add
          ManualIsMoving = true;                                                //✅add
          ManualMoveStartTime = millis();                                        //✅add
          break;                                                                //✅add
      }                                                                         //✅add
      ExecuteMovement(Command);                                                 //✅add
      break;                                                                    //✅add
                                                                                //✅add
    case MODE_TEACH:                                                            //✅add
      Teach_HandleCommand(Command);                                             //✅add
      break;                                                                    //✅add
                                                                                //✅add
    case MODE_LEADER:                                                           //✅add (جديد)
      LF_Leader_HandleCommand(Command);                                         //✅add
      break;                                                                    //✅add
                                                                                //✅add
    default:                                                                    //✅add
      // Autonomous, parking, repeat, and follower modes ignore movement commands //✅add
      break;                                                                    //✅add
  }                                                                             //✅add
}

// ═══════════════════════════════════════════════════════════════════════════════
// MANUAL UPDATE
// ═══════════════════════════════════════════════════════════════════════════════
static void UpdateManualMode(void)
{
  if (ManualIsMoving && (millis() - ManualMoveStartTime >= ManualMoveDuration))
  {
    Motor_Stop();
    ManualIsMoving = false;
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// AUTONOMOUS UPDATE
// ═══════════════════════════════════════════════════════════════════════════════
static void UpdateAutonomousMode(void)
{
  unsigned long now = millis();
  long F = Sensor_GetFrontDistance();
  long B = Sensor_GetBackDistance();
  long R = Sensor_GetRightDistance();
  long L = Sensor_GetLeftDistance();

  bool frontBlocked = DirectionIsBlocked(F);
  bool backBlocked  = DirectionIsBlocked(B);
  bool rightBlocked = DirectionIsBlocked(R);
  bool leftBlocked  = DirectionIsBlocked(L);

  switch (CurrentAutoState)
  {
    case AUTO_DRIVE:
      if (!frontBlocked && !rightBlocked && !leftBlocked)
        Motor_Forward();
      else
      {
        Motor_Stop();
        if      (!leftBlocked)  { SetAutoState(AUTO_AVOID_LEFT);  Motor_Left();     }
        else if (!rightBlocked) { SetAutoState(AUTO_AVOID_RIGHT); Motor_Right();    }
        else if (!backBlocked)  { SetAutoState(AUTO_AVOID_BACK);  Motor_Backward(); }
        else                    { SetAutoState(AUTO_STOPPED);     Motor_Stop();     }
      }
      break;

    case AUTO_AVOID_LEFT:
      Motor_Left();
      if (now - AutoStateStartTime >= AvoidTurnTime) SetAutoState(AUTO_DRIVE);
      break;

    case AUTO_AVOID_RIGHT:
      Motor_Right();
      if (now - AutoStateStartTime >= AvoidTurnTime) SetAutoState(AUTO_DRIVE);
      break;

    case AUTO_AVOID_BACK:
      Motor_Backward();
      if (now - AutoStateStartTime >= AvoidBackTime) SetAutoState(AUTO_DRIVE);
      break;

    case AUTO_STOPPED:
      if (DirectionIsOpen(F) && DirectionIsOpen(B) &&
          DirectionIsOpen(R) && DirectionIsOpen(L))
        SetAutoState(AUTO_DRIVE);
      break;
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// PARKING UPDATE
// ═══════════════════════════════════════════════════════════════════════════════
static void UpdateParkingMode(void)
{
  unsigned long elapsed = millis() - ParkingPhaseStart;
  long rightDist = Sensor_GetRightDistance();
  long backDist  = Sensor_GetBackDistance();

  switch (CurrentParkingState)
  {
    case PARK_FIND_SPOT:
      DriftCorrectedForward();

      if (rightDist != -1)
      {
        if (!ParkSeenSolidFirst)
        {
          if (RightIsSolid()) ParkSeenSolidFirst = true;
        }
        else if (IsSuddenGap(rightDist))
        {
          Motor_Stop();
          delay(300);
          SetParkingState(PARK_OVERSHOOT);
        }
      }
      lastRightDistance = rightDist;
      break;

    case PARK_OVERSHOOT:
      DriftCorrectedForward();
      if (elapsed >= PARK_OVERSHOOT_MS)
      {
        Motor_Stop();
        delay(400);
        SetParkingState(PARK_REVERSE_IN);
      }
      break;

    case PARK_REVERSE_IN:
      Motor_BackwardRight();
      if (BackTooClose() || elapsed >= PARK_REVERSE_IN_MS)
      {
        Motor_Stop();
        delay(200);                                           // ✅add: تقليل التأخير
        SetParkingState(PARK_REVERSE_STRAIGHT);               // ✅add: الانتقال للحركة الجديدة
      }
      break;

    // ✅add: حركة الرجوع للخلف مباشرة (بدون انحراف)
    case PARK_REVERSE_STRAIGHT:
      Motor_Backward();                                       // ✅add: رجوع للخلف مستقيم
      if (elapsed >= PARK_REVERSE_STRAIGHT_MS)                // ✅add
      {
        Motor_Stop();
        delay(200);                                           // ✅add
        SetParkingState(PARK_STRAIGHTEN);                     // ✅add: الانتقال للحركة الأخيرة
      }
      break;  

    case PARK_STRAIGHTEN:
      Motor_BackwardLeft();
      if (BackTooClose() || elapsed >= PARK_STRAIGHTEN_MS)
      {
        Motor_Stop();
        SetParkingState(PARK_DONE);
      }
      break;

    case PARK_DONE:
    case PARK_IDLE:
    default:
      Motor_Stop();
      break;
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN UPDATE
// ═══════════════════════════════════════════════════════════════════════════════
void Modes_Update(void)
{
  if      (CurrentMode == MODE_MANUAL)      UpdateManualMode();
  else if (CurrentMode == MODE_AUTONOMOUS) UpdateAutonomousMode();
  else if (CurrentMode == MODE_PARKING)    UpdateParkingMode();
  else if (CurrentMode == MODE_REPEAT)                                     //✅add
  {                                                                        //✅add
    if (!Repeat_Update())                                                  //✅add
    {                                                                      //✅add
      CurrentMode = MODE_MANUAL;                                           //✅add
      Motor_SetSpeed(speed[4]);                                            //✅add
    }                                                                      //✅add
  }                                                                        //✅add
  else if (CurrentMode == MODE_LEADER)                                     //✅add (جديد)
  {                                                                        //✅add
    LF_Leader_Update();                                                    //✅add
  }                                                                        //✅add
  else if (CurrentMode == MODE_FOLLOWER)                                   //✅add (جديد)
  {                                                                        //✅add
    LF_Follower_Update();                                                  //✅add
  }                                                                        //✅add
  // MODE_TEACH is event-driven, nothing to poll                          //✅add
}