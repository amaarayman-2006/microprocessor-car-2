#include <Arduino.h>
#include "Modes.h"
#include "Config.h"
#include "Motor.h"
#include "Sensor.h"
#include "TeachAndRepeat.h"
#include "Bluetooth.h"

static int speed[] = {80, 100, 120, 140, 160, 180, 200, 225, 250};

static RobotMode CurrentMode = MODE_MANUAL;

// ═══════════════════════════════════════════════════════════════════════════════
// MANUAL MODE
// ═══════════════════════════════════════════════════════════════════════════════

static unsigned long ManualMoveStartTime = 0;
static bool ManualIsMoving = false;
static const int ManualMoveDuration = 600;

// ═══════════════════════════════════════════════════════════════════════════════
// AUTONOMOUS MODE
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum
{
  AUTO_DRIVE,
  AUTO_AVOID_LEFT,
  AUTO_AVOID_RIGHT,
  AUTO_AVOID_BACK,
  AUTO_STOPPED
} AutoState;

static AutoState CurrentAutoState = AUTO_DRIVE;
static unsigned long AutoStateStartTime = 0;

static const int AvoidTurnTime = 700;
static const int AvoidBackTime = 600;

// ═══════════════════════════════════════════════════════════════════════════════
// PARKING MODE
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum
{
  PARK_SEARCH_SPOT = 0,
  PARK_STOP_AFTER_SPOT,
  PARK_SEARCH_TOP_CAR,
  PARK_STOP_AFTER_TOP,
  PARK_TURN_LEFT,
  PARK_BACK_UNTIL_BOTTOM,
  PARK_TURN_RIGHT,
  PARK_DONE
} ParkingState;

static ParkingState CurrentParkingState = PARK_SEARCH_SPOT;
static unsigned long ParkingStateStartTime = 0;

static bool SpotDetected = false;
static bool TopCarDetected = false;

// ═══════════════════════════════════════════════════════════════════════════════
// IMPROVED DETECTION FUNCTIONS (New & More Stable)
// ═══════════════════════════════════════════════════════════════════════════════

// Improved Spot Detection (Empty Space)
static long ReadRightForSpot(void)
{
  const int samples = 6;
  long readings[samples];
  int validCount = 0;

  for (int i = 0; i < samples; i++)
  {
    long d = Sensor_GetRightDistance();
    if (d != -1)
    {
      readings[validCount++] = d;
    }
    delay(7);
  }

  if (validCount == 0) return -1;

  // Sort and return median
  for (int i = 0; i < validCount-1; i++)
    for (int j = i+1; j < validCount; j++)
      if (readings[i] > readings[j])
      {
        long temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }

  return readings[validCount / 2];
}

// Improved Car Detection (Top Car)
static long ReadRightForCar(void)
{
  const int samples = 6;
  long readings[samples];
  int validCount = 0;

  for (int i = 0; i < samples; i++)
  {
    long d = Sensor_GetRightDistance();
    if (d != -1)
    {
      readings[validCount++] = d;
    }
    delay(7);
  }

  if (validCount == 0) return -1;

  // Sort and return median
  for (int i = 0; i < validCount-1; i++)
    for (int j = i+1; j < validCount; j++)
      if (readings[i] > readings[j])
      {
        long temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }

  return readings[validCount / 2];
}

// For detecting bottom car while reversing
static long ReadBackForCar(void)
{
  long d1 = Sensor_GetBackDistance();
  delay(5);
  long d2 = Sensor_GetBackDistance();

  if (d1 == -1 && d2 == -1) return -1;
  else if (d1 == -1) return d2;
  else if (d2 == -1) return d1;

  return (d1 < d2) ? d1 : d2;   // Smaller = more likely car
}

// ═══════════════════════════════════════════════════════════════════════════════
// HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

static void SetAutoState(AutoState NewState)
{
  CurrentAutoState = NewState;
  AutoStateStartTime = millis();
}

static void SetParkingState(ParkingState NewState)
{
  CurrentParkingState = NewState;
  ParkingStateStartTime = millis();
}

static void ResetParkingMode(void)
{
  SpotDetected = false;
  TopCarDetected = false;
  CurrentParkingState = PARK_SEARCH_SPOT;
  ParkingStateStartTime = millis();
}

static bool DirectionIsBlocked(long Distance)
{
  return (Distance != -1 && Distance <= OBSTACLE_DISTANCE);
}

static bool DirectionIsOpen(long Distance)
{
  return (Distance == -1 || Distance > OBSTACLE_DISTANCE);
}

static void ExecuteMovement(char cmd)
{
  switch (cmd)
  {
    case 'F': Motor_Forward(); break;
    case 'B': Motor_Backward(); break;
    case 'L': Motor_Left(); break;
    case 'R': Motor_Right(); break;
    case 'G': Motor_ForwardLeft(); break;
    case 'I': Motor_ForwardRight(); break;
    case 'H': Motor_BackwardLeft(); break;
    case 'J': Motor_BackwardRight(); break;
    case 'S': Motor_Stop(); break;
    default: break;
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// BASIC FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

RobotMode Modes_GetCurrentMode(void)
{
  return CurrentMode;
}

void Modes_Init(void)
{
  CurrentMode = MODE_MANUAL;
  CurrentAutoState = AUTO_DRIVE;
  ResetParkingMode();
  Motor_SetSpeed(speed[4]);
  Motor_Stop();
}

void Modes_SetMode(RobotMode Mode)
{
  Motor_Stop();

  if (CurrentMode == MODE_PARKING && Mode != MODE_PARKING)
  {
    ResetParkingMode();
  }

  if (CurrentMode == MODE_TEACH)
  {
    Teach_Finalise();
  }

  CurrentMode = Mode;

  if (CurrentMode == MODE_MANUAL)
  {
    Motor_SetSpeed(speed[4]);
  }
  else if (CurrentMode == MODE_AUTONOMOUS)
  {
    Motor_SetSpeed(AUTO_SPEED);
    SetAutoState(AUTO_DRIVE);
  }
  else if (CurrentMode == MODE_PARKING)
  {
    Motor_SetSpeed(PARK_SPEED);
    ResetParkingMode();
    Motor_Stop();
  }
  else if (CurrentMode == MODE_TEACH)
  {
    Motor_SetSpeed(speed[4]);
    Teach_Init();
  }
  else if (CurrentMode == MODE_REPEAT)
  {
    if (!Repeat_Init())
    {
      CurrentMode = MODE_MANUAL;
      Motor_SetSpeed(speed[4]);
    }
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMMAND HANDLER
// ═══════════════════════════════════════════════════════════════════════════════

void Modes_HandleCommand(char Command)
{
  switch (Command)
  {
    case 'U': Modes_SetMode(MODE_MANUAL); return;
    case 'W': Modes_SetMode(MODE_AUTONOMOUS); return;
    case 'X': Modes_SetMode(MODE_PARKING); return;
    case 'T': Modes_SetMode(MODE_TEACH); return;
    case 'P': Modes_SetMode(MODE_REPEAT); return;
    case 'K': Modes_SetMode(MODE_LEADER); return;
    case 'Z': Modes_SetMode(MODE_FOLLOWER); return;
  }

  if (Command == 'S')
  {
    if (CurrentMode == MODE_TEACH)
    {
      Teach_Finalise();
    }

    Motor_Stop();
    CurrentMode = MODE_MANUAL;
    CurrentAutoState = AUTO_DRIVE;
    ResetParkingMode();
    ManualIsMoving = false;
    return;
  }

  if (CurrentMode == MODE_MANUAL)
  {
    if (Command >= '1' && Command <= '9')
    {
      Motor_SetSpeed(speed[Command - '1']);
      return;
    }

    if (Command == 'F' || Command == 'B' || Command == 'L' || Command == 'R' ||
        Command == 'G' || Command == 'H' || Command == 'I' || Command == 'J')
    {
      ManualIsMoving = true;
      ManualMoveStartTime = millis();
    }

    ExecuteMovement(Command);
  }
  else if (CurrentMode == MODE_TEACH)
  {
    Teach_HandleCommand(Command);
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// UPDATE FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

static void UpdateManualMode(void)
{
  if (ManualIsMoving && millis() - ManualMoveStartTime >= ManualMoveDuration)
  {
    Motor_Stop();
    ManualIsMoving = false;
  }
}

static void UpdateAutonomousMode(void)
{
  unsigned long now = millis();

  long F = Sensor_GetFrontDistance();
  long B = Sensor_GetBackDistance();
  long R = Sensor_GetRightDistance();
  long L = Sensor_GetLeftDistance();

  bool frontBlocked = DirectionIsBlocked(F);
  bool backBlocked = DirectionIsBlocked(B);
  bool rightBlocked = DirectionIsBlocked(R);
  bool leftBlocked = DirectionIsBlocked(L);

  switch (CurrentAutoState)
  {
    case AUTO_DRIVE:
      if (!frontBlocked && !rightBlocked && !leftBlocked)
      {
        Motor_Forward();
      }
      else
      {
        Motor_Stop();
        if (!leftBlocked)
        {
          SetAutoState(AUTO_AVOID_LEFT);
          Motor_Left();
        }
        else if (!rightBlocked)
        {
          SetAutoState(AUTO_AVOID_RIGHT);
          Motor_Right();
        }
        else if (!backBlocked)
        {
          SetAutoState(AUTO_AVOID_BACK);
          Motor_Backward();
        }
        else
        {
          SetAutoState(AUTO_STOPPED);
          Motor_Stop();
        }
      }
      break;

    case AUTO_AVOID_LEFT:
      Motor_Left();
      if (now - AutoStateStartTime >= AvoidTurnTime)
        SetAutoState(AUTO_DRIVE);
      break;

    case AUTO_AVOID_RIGHT:
      Motor_Right();
      if (now - AutoStateStartTime >= AvoidTurnTime)
        SetAutoState(AUTO_DRIVE);
      break;

    case AUTO_AVOID_BACK:
      Motor_Backward();
      if (now - AutoStateStartTime >= AvoidBackTime)
        SetAutoState(AUTO_DRIVE);
      break;

    case AUTO_STOPPED:
      Motor_Stop();
      if (DirectionIsOpen(F) && DirectionIsOpen(B) &&
          DirectionIsOpen(R) && DirectionIsOpen(L))
      {
        SetAutoState(AUTO_DRIVE);
      }
      break;
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// PARKING MODE (Improved Detection)
// ═══════════════════════════════════════════════════════════════════════════════

static void UpdateParkingMode(void)
{
  switch (CurrentParkingState)
  {
    case PARK_SEARCH_SPOT:
    {
      long rightDist = ReadRightForSpot();
      Motor_Forward();

      if (rightDist != -1 && rightDist >= PARK_SPOT_DETECT_CM)
      {
        SpotDetected = true;
        Motor_Stop();
        SetParkingState(PARK_STOP_AFTER_SPOT);
      }
      break;
    }

    case PARK_STOP_AFTER_SPOT:
    {
      Motor_Stop();
      if (millis() - ParkingStateStartTime >= PARK_STOP_DELAY_MS)
      {
        SetParkingState(PARK_SEARCH_TOP_CAR);
      }
      break;
    }

    case PARK_SEARCH_TOP_CAR:
    {
      long rightDist = ReadRightForCar();
      Motor_Forward();

      if (rightDist != -1 && rightDist <= PARK_RIGHT_CAR_DETECT_CM)
      {
        TopCarDetected = true;
        Motor_Stop();
        SetParkingState(PARK_STOP_AFTER_TOP);
      }
      break;
    }

    case PARK_STOP_AFTER_TOP:
    {
      Motor_Stop();
      if (millis() - ParkingStateStartTime >= PARK_STOP_DELAY_MS)
      {
        if (SpotDetected && TopCarDetected)
        {
          SetParkingState(PARK_TURN_LEFT);
        }
      }
      break;
    }

    case PARK_TURN_LEFT:
    {
      Motor_Left();
      if (millis() - ParkingStateStartTime >= PARK_TURN_TIME_MS)
      {
        Motor_Stop();
        SetParkingState(PARK_BACK_UNTIL_BOTTOM);
      }
      break;
    }

    case PARK_BACK_UNTIL_BOTTOM:
    {
      long backDist = ReadBackForCar();
      Motor_Backward();

      if (backDist != -1 && backDist <= PARK_BACK_CAR_DETECT_CM)
      {
        Motor_Stop();
        SetParkingState(PARK_TURN_RIGHT);
      }
      break;
    }

    case PARK_TURN_RIGHT:
    {
      Motor_Right();
      if (millis() - ParkingStateStartTime >= PARK_TURN_TIME_MS2)
      {
        Motor_Stop();
        SetParkingState(PARK_DONE);
      }
      break;
    }

    case PARK_DONE:
    default:
    {
      Motor_Stop();
      CurrentMode = MODE_MANUAL;
      Motor_SetSpeed(speed[4]);
      ResetParkingMode();
      break;
    }
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
// MAIN UPDATE
// ═══════════════════════════════════════════════════════════════════════════════

void Modes_Update(void)
{
  if (CurrentMode == MODE_MANUAL)
  {
    UpdateManualMode();
  }
  else if (CurrentMode == MODE_AUTONOMOUS)
  {
    UpdateAutonomousMode();
  }
  else if (CurrentMode == MODE_PARKING)
  {
    UpdateParkingMode();
  }
  else if (CurrentMode == MODE_REPEAT)
  {
    if (!Repeat_Update())
    {
      CurrentMode = MODE_MANUAL;
      Motor_SetSpeed(speed[4]);
      ResetParkingMode();
    }
  }
}