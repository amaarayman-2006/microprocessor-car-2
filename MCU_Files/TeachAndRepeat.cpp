#include <Arduino.h>
#include "TeachAndRepeat.h"
#include "Config.h"
#include "Motor.h"
#include "Sensor.h"
#include "Bluetooth.h"

// ── Data types ──────────────────────────────────────────────────────────────── //✅add

// One recorded movement segment.                                                //✅add
typedef struct                                                                   //✅add
{                                                                                //✅add
  char     Command;      // F/B/L/R/G/H/I/J/S or 'V' for speed                   //✅add
  uint16_t Value;        // for 'V': speed value (80-250), for movement: duration //✅add
} TeachSegment;                                                                  //✅add

// ── Module-private state ────────────────────────────────────────────────────── //✅add

// ✅add: زيادة المساحة من 100 إلى 200 سجل
static TeachSegment  TeachPath[200];                                             //✅add (كانت 100)
static uint8_t       TeachCount = 0;                                             //✅add

// Recording helpers (TEACH phase).                                              //✅add
static char          TeachLastCmd  = '\0';                                       //✅add
static unsigned long TeachCmdStart = 0;                                          //✅add

// Playback helpers (REPEAT phase).                                              //✅add
static uint8_t       RepeatIndex      = 0;                                       //✅add
static unsigned long RepeatPhaseStart = 0;                                       //✅add
static bool          RepeatPaused     = false;                                   //✅add

// Speed table (same as in Modes.cpp)                                            //✅add
static const int SpeedTable[] = {80, 100, 120, 140, 160, 180, 200, 225, 250};    //✅add

// ── Internal helpers ────────────────────────────────────────────────────────── //✅add

// Drive the motor for a single movement character.                             //✅add
static void ExecuteMovement(char cmd)                                            //✅add
{                                                                                //✅add
  switch (cmd)                                                                   //✅add
  {                                                                              //✅add
    case 'F': Motor_Forward();       break;                                      //✅add
    case 'B': Motor_Backward();      break;                                      //✅add
    case 'L': Motor_Left();          break;                                      //✅add
    case 'R': Motor_Right();         break;                                      //✅add
    case 'G': Motor_ForwardLeft();   break;                                      //✅add
    case 'H': Motor_ForwardRight();  break;                                      //✅add
    case 'I': Motor_BackwardLeft();  break;                                      //✅add
    case 'J': Motor_BackwardRight(); break;                                      //✅add
    case 'S': Motor_Stop();          break;                                      //✅add
    default:                         break;                                      //✅add
  }                                                                              //✅add
}                                                                                //✅add

// Returns true if cmd is a recordable movement primitive.                       //✅add
static bool IsMovementCommand(char cmd)                                          //✅add
{                                                                                //✅add
  return (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R' ||                //✅add
          cmd == 'G' || cmd == 'H' || cmd == 'I' || cmd == 'J' ||                //✅add
          cmd == 'S');                                                           //✅add
}                                                                                //✅add

// Returns true if cmd is a speed command.                                       //✅add
static bool IsSpeedCommand(char cmd)                                             //✅add
{                                                                                //✅add
  return (cmd >= '1' && cmd <= '9');                                             //✅add
}                                                                                //✅add

// Save a movement segment (command + duration)                                 //✅add
static bool SaveMovementSegment(char cmd, unsigned long duration)               //✅add
{                                                                                //✅add
  if (duration < TEACH_MIN_DURATION_MS) return true;                             //✅add
  // ✅add: زيادة الحد الأقصى من 100 إلى 200
  if (TeachCount >= 200)                                                         //✅add (كانت TEACH_MAX_SEGMENTS)
  {                                                                              //✅add
    Bluetooth_Send("TEACH:FULL");                                                //✅add
    return false;                                                                //✅add
  }                                                                              //✅add
                                                                                 //✅add
  TeachPath[TeachCount].Command = cmd;                                           //✅add
  TeachPath[TeachCount].Value    = (uint16_t)min(duration, (unsigned long)65535UL); //✅add
  TeachCount++;                                                                  //✅add
  Bluetooth_Send("TEACH:SAVED");                                                 //✅add
  return true;                                                                   //✅add
}                                                                                //✅add

// Save a speed segment                                                          //✅add
static void SaveSpeedSegment(int speedValue)                                     //✅add
{                                                                                //✅add
  // ✅add: زيادة الحد الأقصى من 100 إلى 200
  if (TeachCount >= 200)                                                         //✅add (كانت TEACH_MAX_SEGMENTS)
  {                                                                              //✅add
    Bluetooth_Send("TEACH:FULL");                                                //✅add
    return;                                                                      //✅add
  }                                                                              //✅add
                                                                                 //✅add
  // 'V' = Velocity/Speed command                                                //✅add
  TeachPath[TeachCount].Command = 'V';                                           //✅add
  TeachPath[TeachCount].Value    = (uint16_t)speedValue;                         //✅add
  TeachCount++;                                                                  //✅add
  char msg[20];                                                                  //✅add
  sprintf(msg, "TEACH:SPD=%d", speedValue);                                      //✅add
  Bluetooth_Send(msg);                                                           //✅add
}                                                                                //✅add

// ═══════════════════════════════════════════════════════════════════════════════ //✅add
// TEACH PHASE – public API                                                        //✅add
// ═══════════════════════════════════════════════════════════════════════════════ //✅add

void Teach_Init(void)                                                            //✅add
{                                                                                //✅add
  TeachCount    = 0;                                                             //✅add
  TeachLastCmd  = '\0';                                                          //✅add
  TeachCmdStart = millis();                                                      //✅add
                                                                                 //✅add
  Motor_Stop();                                                                  //✅add
  Bluetooth_Send("TEACH:OK");                                                    //✅add
}                                                                                //✅add

void Teach_HandleCommand(char Command)                                           //✅add
{                                                                                //✅add
  unsigned long now = millis();                                                  //✅add
  
  // ── Handle Speed Changes (record as special segments) ─────────────────────   //✅add
  if (IsSpeedCommand(Command))                                                   //✅add
  {                                                                              //✅add
    int newSpeed = SpeedTable[Command - '1'];                                    //✅add
    Motor_SetSpeed(newSpeed);                                                    //✅add
    SaveSpeedSegment(newSpeed);                                                  //✅add
    return;                                                                      //✅add
  }                                                                              //✅add
  
  // ── Handle Movement Commands ───────────────────────────────────────────────   //✅add
  if (!IsMovementCommand(Command)) return;                                       //✅add
  
  // If the command has changed, flush the previous one.                        //✅add
  if (TeachLastCmd != '\0' && TeachLastCmd != Command)                           //✅add
  {                                                                              //✅add
    unsigned long held = now - TeachCmdStart;                                    //✅add
    if (!SaveMovementSegment(TeachLastCmd, held))                                //✅add
    {                                                                            //✅add
      TeachLastCmd = '\0';                                                       //✅add
      return;                                                                    //✅add
    }                                                                            //✅add
  }                                                                              //✅add
  
  // Start (or continue) timing the new command.                                //✅add
  if (TeachLastCmd != Command)                                                   //✅add
  {                                                                              //✅add
    TeachLastCmd  = Command;                                                     //✅add
    TeachCmdStart = now;                                                         //✅add
  }                                                                              //✅add
  
  // Drive the car live so the operator can see the path.                       //✅add
  ExecuteMovement(Command);                                                      //✅add
}                                                                                //✅add

void Teach_Finalise(void)                                                        //✅add
{                                                                                //✅add
  if (TeachLastCmd == '\0') return;                                              //✅add
                                                                                 //✅add
  unsigned long held = millis() - TeachCmdStart;                                 //✅add
  SaveMovementSegment(TeachLastCmd, held);                                       //✅add
  TeachLastCmd = '\0';                                                           //✅add
}                                                                                //✅add

uint8_t Teach_GetCount(void)                                                     //✅add
{                                                                                //✅add
  return TeachCount;                                                             //✅add
}                                                                                //✅add

// ═══════════════════════════════════════════════════════════════════════════════ //✅add
// REPEAT PHASE – public API                                                       //✅add
// ═══════════════════════════════════════════════════════════════════════════════ //✅add

bool Repeat_Init(void)                                                           //✅add
{                                                                                //✅add
  if (TeachCount == 0)                                                           //✅add
  {                                                                              //✅add
    Bluetooth_Send("REPEAT:EMPTY");                                              //✅add
    return false;                                                                //✅add
  }                                                                              //✅add
                                                                                 //✅add
  // Start with default speed (will be overridden if first segment is speed)    //✅add
  Motor_SetSpeed(REPEAT_SPEED);                                                  //✅add
  Motor_Stop();                                                                  //✅add
                                                                                 //✅add
  RepeatIndex      = 0;                                                          //✅add
  RepeatPhaseStart = millis();                                                   //✅add
  RepeatPaused     = false;                                                      //✅add
                                                                                 //✅add
  // Start playback from the first segment                                       //✅add
  if (TeachPath[0].Command == 'V')                                               //✅add
  {                                                                              //✅add
    // First segment is speed                                                    //✅add
    Motor_SetSpeed(TeachPath[0].Value);                                          //✅add
    RepeatIndex++;                                                               //✅add
  }                                                                              //✅add
  else                                                                           //✅add
  {                                                                              //✅add
    ExecuteMovement(TeachPath[0].Command);                                       //✅add
  }                                                                              //✅add
                                                                                 //✅add
  Bluetooth_Send("REPEAT:OK");                                                   //✅add
  return true;                                                                   //✅add
}                                                                                //✅add

bool Repeat_Update(void)                                                         //✅add
{                                                                                //✅add
  // ── All segments finished ─────────────────────────────────────────────────   //✅add
  if (RepeatIndex >= TeachCount)                                                 //✅add
  {                                                                              //✅add
    Motor_Stop();                                                                //✅add
    Bluetooth_Send("REPEAT:DONE");                                               //✅add
    return false;                                                                //✅add
  }                                                                              //✅add
  
  // ✅add: تم إزالة جزء فحص العوائق الأمامية بالكامل
  // ❌ تم حذف الكود التالي:
  // long frontDist = Sensor_GetFrontDistance();
  // bool frontObstacle = (frontDist != -1 && frontDist <= REPEAT_OBSTACLE_CM);
  // if (frontObstacle && !RepeatPaused) { ... }
  
  // ✅add: إلغاء نظام التوقف المؤقت (Pause) نهائياً
  // التكرار يعمل بدون توقف بغض النظر عن العوائق
  // لأن المسار المسجل صحيح أصلاً
  
  // ── Handle Speed segment (instant, no duration) ───────────────────────────   //✅add
  if (TeachPath[RepeatIndex].Command == 'V')                                     //✅add
  {                                                                              //✅add
    Motor_SetSpeed(TeachPath[RepeatIndex].Value);                                //✅add
    RepeatIndex++;                                                               //✅add
    RepeatPhaseStart = millis();                                                 //✅add
    return true;                                                                 //✅add
  }                                                                              //✅add
  
  // ── Advance to the next segment when the current one has elapsed ──────────   //✅add
  unsigned long elapsed = millis() - RepeatPhaseStart;                           //✅add
  
  if (elapsed >= TeachPath[RepeatIndex].Value)                                   //✅add
  {                                                                              //✅add
    RepeatIndex++;                                                               //✅add
                                                                                 //✅add
    if (RepeatIndex >= TeachCount)                                               //✅add
    {                                                                            //✅add
      Motor_Stop();                                                              //✅add
      Bluetooth_Send("REPEAT:DONE");                                             //✅add
      return false;                                                              //✅add
    }                                                                            //✅add
                                                                                 //✅add
    RepeatPhaseStart = millis();                                                 //✅add
                                                                                 //✅add
    // Skip any speed segments at the start of remaining playback               //✅add
    while (RepeatIndex < TeachCount && TeachPath[RepeatIndex].Command == 'V')    //✅add
    {                                                                            //✅add
      Motor_SetSpeed(TeachPath[RepeatIndex].Value);                              //✅add
      RepeatIndex++;                                                             //✅add
    }                                                                            //✅add
                                                                                 //✅add
    if (RepeatIndex < TeachCount)                                                //✅add
    {                                                                            //✅add
      ExecuteMovement(TeachPath[RepeatIndex].Command);                           //✅add
    }                                                                            //✅add
  }                                                                              //✅add
  
  return true;                                                                   //✅add
}                                                                                //✅add