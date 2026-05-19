#ifndef LEADER_FOLLOWER_H
#define LEADER_FOLLOWER_H

#include <stdint.h>
#include "LeaderFollowerConfig.h"

// ══════════════════════════════════════════════════════════════════════════════
// LeaderFollower.h - نظام القائد والتابع (Leader-Follower)
//
// يعمل هذا النظام على:
//   1. القائد (LEADER): يرسل أوامر الحركة والسرعة والمسافات إلى التابع
//   2. التابع (FOLLOWER): يستقبل الأوامر ويحافظ على مسافة آمنة
//
// تنسيق الحزمة المرسلة: LF:<cmd>,<speed>,<front>,<back>,<right>,<left>\n
// مثال: LF:F,160,42,180,95,110\n
// ══════════════════════════════════════════════════════════════════════════════

// ── دوال القائد (Leader) ─────────────────────────────────────────────────────
void LF_Leader_Init(void);                                    // ✅add
void LF_Leader_HandleCommand(char cmd);                       // ✅add
void LF_Leader_Update(void);                                  // ✅add

// ── دوال التابع (Follower) ───────────────────────────────────────────────────
void LF_Follower_Init(void);                                  // ✅add
void LF_Follower_Update(void);                                // ✅add

// ── دوال مشتركة ──────────────────────────────────────────────────────────────
void LF_ExecuteMovement(char cmd);                            // ✅add
void LF_SetSpeed(int speed);                                  // ✅add

#endif  // LEADER_FOLLOWER_H