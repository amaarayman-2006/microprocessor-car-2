#ifndef TEACH_AND_REPEAT_H
#define TEACH_AND_REPEAT_H

#include <stdint.h>
#include "Config.h"

// ── Teach-and-Repeat mode ──────────────────────────────────────────────────── //✅add
//                                                                              //✅add
// TEACH phase  (MODE_TEACH)                                                    //✅add
//   The operator drives the car manually via Bluetooth while every movement   //✅add
//   command and its hold-duration are recorded into a SRAM buffer.            //✅add
//   Each segment costs 3 bytes; 200 segments = 600 bytes.                     //✅add (كانت 300 bytes)
//   Speed changes ('1'..'9') are also recorded as special segments.           //✅add
//                                                                              //✅add
// REPEAT phase  (MODE_REPEAT)                                                  //✅add
//   The car replays every recorded segment in order using millis()-based      //✅add
//   timing (no blocking delay).                                               //✅add
//   ✅ تم إزالة نظام اكتشاف العوائق أثناء التكرار                              //✅add
//                                                                              //✅add
// Commands  (app → car):                                                       //✅add
//   'T'  – enter teach/record mode                                            //✅add
//   'P'  – start playback (repeat) mode                                       //✅add
//   'S'  – stop / exit to manual   (handled by Modes layer)                   //✅add
// ─────────────────────────────────────────────────────────────────────────────//

// ── TEACH phase ─────────────────────────────────────────────────────────────── //✅add
void Teach_Init(void);                                                           //✅add
void Teach_HandleCommand(char Command);                                          //✅add
void Teach_Finalise(void);                                                       //✅add
uint8_t Teach_GetCount(void);                                                    //✅add

// ── REPEAT phase ────────────────────────────────────────────────────────────── //✅add
bool Repeat_Init(void);                                                          //✅add
bool Repeat_Update(void);                                                        //✅add

#endif