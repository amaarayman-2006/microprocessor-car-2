#ifndef MODES_H
#define MODES_H

// ── Operating modes ───────────────────────────────────────────────────────────
typedef enum
{
  MODE_MANUAL        = 1,
  MODE_AUTONOMOUS    = 2,
  MODE_PARKING       = 3,
  MODE_TEACH         = 4,   // Teach-and-Repeat: record  //✅add
  MODE_REPEAT        = 5,   // Teach-and-Repeat: playback //✅add
  MODE_LEADER        = 6,   // Leader-Follower: Leader mode //✅add
  MODE_FOLLOWER      = 7    // Leader-Follower: Follower mode //✅add
} RobotMode;

// ── Public API ────────────────────────────────────────────────────────────────
void      Modes_Init(void);
void      Modes_SetMode(RobotMode Mode);
RobotMode Modes_GetCurrentMode(void);
void      Modes_HandleCommand(char Command);
void      Modes_Update(void);

#endif