#include <Arduino.h>
#include "LeaderFollower.h"
#include "Config.h"
#include "Motor.h"
#include "Sensor.h"
#include "Bluetooth.h"

// ══════════════════════════════════════════════════════════════════════════════
// دوال مشتركة (Shared Functions)
// ══════════════════════════════════════════════════════════════════════════════

// جدول السرعات (نفس جدول Modes.cpp) //✅add
static const int SpeedTable[] = {80, 100, 120, 140, 160, 180, 200, 225, 250};  //✅add

// تنفيذ أمر حركة //✅add
void LF_ExecuteMovement(char cmd)                                              //✅add
{                                                                              //✅add
  switch (cmd)                                                                 //✅add
  {                                                                            //✅add
    case 'F': Motor_Forward();       break;                                    //✅add
    case 'B': Motor_Backward();      break;                                    //✅add
    case 'L': Motor_Left();          break;                                    //✅add
    case 'R': Motor_Right();         break;                                    //✅add
    case 'G': Motor_ForwardLeft();   break;                                    //✅add
    case 'H': Motor_ForwardRight();  break;                                    //✅add
    case 'I': Motor_BackwardLeft();  break;                                    //✅add
    case 'J': Motor_BackwardRight(); break;                                    //✅add
    case 'S': Motor_Stop();          break;                                    //✅add
    default:                         break;                                    //✅add
  }                                                                              //✅add
}                                                                                //✅add

// ضبط السرعة //✅add
void LF_SetSpeed(int speed)                                                    //✅add
{                                                                              //✅add
  Motor_SetSpeed(speed);                                                       //✅add
}                                                                              //✅add

// ══════════════════════════════════════════════════════════════════════════════
// القائد (LEADER) - يرسل الأوامر والبيانات
// ══════════════════════════════════════════════════════════════════════════════

#if VEHICLE_ROLE == ROLE_LEADER                                                //✅add

static char          Leader_CurrentCmd       = 'S';                           //✅add
static int           Leader_CurrentSpeed     = LF_LEADER_SPEED;               //✅add
static unsigned long Leader_LastBroadcastMs  = 0;                             //✅add

// بناء وإرسال الحزمة للتابع //✅add
static void Leader_Broadcast(void)                                            //✅add
{                                                                              //✅add
  // قراءة جميع الحساسات //✅add
  long f = Sensor_GetFrontDistance();                                          //✅add
  long b = Sensor_GetBackDistance();                                           //✅add
  long r = Sensor_GetRightDistance();                                          //✅add
  long l = Sensor_GetLeftDistance();                                           //✅add
  
  // تنسيق الحزمة: LF:<cmd>,<speed>,<f>,<b>,<r>,<l>\n //✅add
  char buf[LF_SERIAL_BUF_SIZE];                                                //✅add
  snprintf(buf, sizeof(buf), "LF:%c,%d,%ld,%ld,%ld,%ld\n",                    //✅add
           Leader_CurrentCmd, Leader_CurrentSpeed, f, b, r, l);               //✅add
           
  Serial.print(buf);  // إرسال عبر البلوتوث للتابع //✅add
}                                                                              //✅add

// تهيئة القائد //✅add
void LF_Leader_Init(void)                                                      //✅add
{                                                                              //✅add
  Motor_SetSpeed(LF_LEADER_SPEED);                                             //✅add
  Motor_Stop();                                                                //✅add
  Leader_CurrentCmd = 'S';                                                     //✅add
  Leader_LastBroadcastMs = millis();                                           //✅add
  Bluetooth_Send("LF:LEADER:READY");                                           //✅add
}                                                                              //✅add

// معالجة أوامر القائد (تُستدعى عند استقبال أمر من التطبيق) //✅add
void LF_Leader_HandleCommand(char cmd)                                         //✅add
{                                                                              //✅add
  // معالجة أوامر السرعة (1-9) //✅add
  if (cmd >= '1' && cmd <= '9')                                                //✅add
  {                                                                            //✅add
    int newSpeed = SpeedTable[cmd - '1'];                                      //✅add
    Leader_CurrentSpeed = newSpeed;                                            //✅add
    Motor_SetSpeed(newSpeed);                                                  //✅add
    return;                                                                    //✅add
  }                                                                            //✅add
  
  // التحقق من صحة الأمر //✅add
  if (cmd != 'F' && cmd != 'B' && cmd != 'L' && cmd != 'R' &&                  //✅add
      cmd != 'G' && cmd != 'H' && cmd != 'I' && cmd != 'J' && cmd != 'S')      //✅add
  {                                                                            //✅add
    return;                                                                    //✅add
  }                                                                            //✅add
  
  // تحديث الأمر الحالي وتنفيذه //✅add
  Leader_CurrentCmd = cmd;                                                     //✅add
  LF_ExecuteMovement(cmd);                                                     //✅add
}                                                                              //✅add

// تحديث القائد (يُستدعى كل loop) //✅add
void LF_Leader_Update(void)                                                    //✅add
{                                                                              //✅add
  unsigned long now = millis();                                                //✅add
  
  // إرسال الحزمة بشكل دوري //✅add
  if (now - Leader_LastBroadcastMs >= LF_BROADCAST_INTERVAL_MS)                //✅add
  {                                                                            //✅add
    Leader_LastBroadcastMs = now;                                              //✅add
    Leader_Broadcast();                                                        //✅add
  }                                                                              //✅add
}                                                                                //✅add

// دالة فارغة للـ Follower (لتجنب أخطاء الـ Linker) //✅add
void LF_Follower_Init(void) {}                                                 //✅add
void LF_Follower_Update(void) {}                                               //✅add

#endif  // VEHICLE_ROLE == ROLE_LEADER                                         //✅add

// ══════════════════════════════════════════════════════════════════════════════
// التابع (FOLLOWER) - يستقبل الأوامر ويحافظ على المسافة
// ملاحظة: هذا الكود لن يُترجم في عربيتك (لأن VEHICLE_ROLE = LEADER)
// ولكنه موجود للتوثيق وللاستخدام المستقبلي
// ══════════════════════════════════════════════════════════════════════════════

#if VEHICLE_ROLE == ROLE_FOLLOWER                                                //✅add

static char          Follower_SerialBuf[LF_SERIAL_BUF_SIZE];                   //✅add
static uint8_t       Follower_BufIdx         = 0;                              //✅add
static char          Follower_LeaderCmd      = 'S';                            //✅add
static int           Follower_LeaderSpeed    = LF_FOLLOWER_SPEED;              //✅add
static unsigned long Follower_LastPacketMs   = 0;                              //✅add
static bool          Follower_LeaderLost     = false;                          //✅add

// تحليل السطر المستلم //✅add
static bool Follower_ParseLine(void)                                           //✅add
{                                                                              //✅add
  // التحقق من وجود "LF:" في بداية السطر //✅add
  if (Follower_SerialBuf[0] != 'L' ||                                          //✅add
      Follower_SerialBuf[1] != 'F' ||                                          //✅add
      Follower_SerialBuf[2] != ':')                                            //✅add
  {                                                                            //✅add
    return false;                                                              //✅add
  }                                                                            //✅add
  
  // استخراج الأمر (الموجود بعد "LF:" مباشرة) //✅add
  char cmd = Follower_SerialBuf[3];                                            //✅add
  if (cmd != 'F' && cmd != 'B' && cmd != 'L' && cmd != 'R' &&                  //✅add
      cmd != 'G' && cmd != 'H' && cmd != 'I' && cmd != 'J' && cmd != 'S')      //✅add
  {                                                                            //✅add
    return false;                                                              //✅add
  }                                                                            //✅add
  
  // استخراج السرعة (بعد الأمر وقبل أول فاصلة) //✅add
  int speed = 0;                                                               //✅add
  int idx = 4;  // بعد "LF:X," //✅add
  while (idx < LF_SERIAL_BUF_SIZE && Follower_SerialBuf[idx] != ',' &&         //✅add
         Follower_SerialBuf[idx] != '\0')                                      //✅add
  {                                                                            //✅add
    if (Follower_SerialBuf[idx] >= '0' && Follower_SerialBuf[idx] <= '9')      //✅add
    {                                                                          //✅add
      speed = speed * 10 + (Follower_SerialBuf[idx] - '0');                    //✅add
    }                                                                          //✅add
    idx++;                                                                     //✅add
  }                                                                            //✅add
  
  // تحديث المتغيرات //✅add
  Follower_LeaderCmd = cmd;                                                    //✅add
  if (speed > 0 && speed <= 255) {                                             //✅add
    Follower_LeaderSpeed = speed;                                              //✅add
  }                                                                            //✅add
  Follower_LastPacketMs = millis();                                            //✅add
  
  return true;                                                                 //✅add
}                                                                              //✅add

// التحكم في الفجوة (Gap Control) //✅add
static bool Follower_GapControl(void)                                          //✅add
{                                                                              //✅add
  long dist = Sensor_GetFrontDistance();                                       //✅add
  
  // إذا كان الحساس لا يرى شيئاً → ننفذ أمر القائد //✅add
  if (dist == -1) return true;                                                 //✅add
  
  // طوارئ: الشيء قريب جداً → توقف فوري //✅add
  if (dist <= LF_TOO_CLOSE_CM)                                                 //✅add
  {                                                                            //✅add
    Motor_Brake();                                                             //✅add
    return false;                                                              //✅add
  }                                                                            //✅add
  
  const long lower = (long)LF_TARGET_CM - (long)LF_DEADBAND_CM;                //✅add
  const long upper = (long)LF_TARGET_CM + (long)LF_DEADBAND_CM;                //✅add
  
  if (dist < lower)                                                            //✅add
  {                                                                            //✅add
    // قريب جداً → ابتعد للخلف //✅add
    Motor_Backward();                                                          //✅add
    return false;                                                              //✅add
  }                                                                            //✅add
  else if (dist <= upper)                                                      //✅add
  {                                                                            //✅add
    // في المنطقة الميتة → توقف //✅add
    Motor_Stop();                                                              //✅add
    return false;                                                              //✅add
  }                                                                            //✅add
                                                                               //✅add
  // بعيد جداً → ننفذ أمر القائد //✅add
  return true;                                                                 //✅add
}                                                                              //✅add

// تهيئة التابع //✅add
void LF_Follower_Init(void)                                                    //✅add
{                                                                              //✅add
  Motor_SetSpeed(LF_FOLLOWER_SPEED);                                           //✅add
  Motor_Stop();                                                                //✅add
  Follower_BufIdx = 0;                                                         //✅add
  Follower_LeaderLost = false;                                                 //✅add
  Follower_LastPacketMs = millis();                                            //✅add
  Bluetooth_Send("LF:FOLLOWER:READY");                                         //✅add
}                                                                              //✅add

// تحديث التابع (يُستدعى كل loop) //✅add
void LF_Follower_Update(void)                                                  //✅add
{                                                                              //✅add
  unsigned long now = millis();                                                //✅add
  
  // ── 1. قراءة البيانات من الـ Serial ───────────────────────────────────── //✅add
  while (Serial.available() > 0)                                               //✅add
  {                                                                            //✅add
    char c = (char)Serial.read();                                              //✅add
    
    if (c == '\n' || c == '\r')                                                //✅add
    {                                                                          //✅add
      if (Follower_BufIdx > 0)                                                 //✅add
      {                                                                        //✅add
        Follower_SerialBuf[Follower_BufIdx] = '\0';                            //✅add
        if (Follower_ParseLine())                                              //✅add
        {                                                                      //✅add
          if (Follower_LeaderLost)                                             //✅add
          {                                                                    //✅add
            Follower_LeaderLost = false;                                       //✅add
            Bluetooth_Send("LF:FOUND");                                        //✅add
          }                                                                    //✅add
        }                                                                      //✅add
        Follower_BufIdx = 0;                                                   //✅add
      }                                                                        //✅add
    }                                                                          //✅add
    else                                                                       //✅add
    {                                                                          //✅add
      if (Follower_BufIdx < (LF_SERIAL_BUF_SIZE - 1))                          //✅add
      {                                                                        //✅add
        Follower_SerialBuf[Follower_BufIdx] = c;                               //✅add
        Follower_BufIdx++;                                                     //✅add
      }                                                                        //✅add
      else                                                                     //✅add
      {                                                                        //✅add
        Follower_BufIdx = 0;  // سطر طويل جداً → تجاهل //✅add
      }                                                                        //✅add
    }                                                                          //✅add
  }                                                                            //✅add
  
  // ── 2. مؤقت فقدان الإشارة (Watchdog) ───────────────────────────────────── //✅add
  if (!Follower_LeaderLost &&                                                  //✅add
      (now - Follower_LastPacketMs >= LF_LOST_TIMEOUT_MS))                     //✅add
  {                                                                            //✅add
    Follower_LeaderLost = true;                                                //✅add
    Motor_Stop();                                                              //✅add
    Bluetooth_Send("LF:LOST");                                                 //✅add
    return;                                                                    //✅add
  }                                                                            //✅add
  
  if (Follower_LeaderLost)                                                     //✅add
  {                                                                            //✅add
    Motor_Stop();                                                              //✅add
    return;                                                                    //✅add
  }                                                                            //✅add
  
  // ── 3. التحكم في الفجوة وتنفيذ الأمر ──────────────────────────────────── //✅add
  if (Follower_GapControl())                                                   //✅add
  {                                                                            //✅add
    LF_ExecuteMovement(Follower_LeaderCmd);                                    //✅add
    Motor_SetSpeed(Follower_LeaderSpeed);  // مزامنة السرعة مع القائد //✅add
  }                                                                            //✅add
}                                                                              //✅add

// دالة فارغة للقائد (لتجنب أخطاء الـ Linker) //✅add
void LF_Leader_Init(void) {}                                                   //✅add
void LF_Leader_HandleCommand(char cmd) { (void)cmd; }                          //✅add
void LF_Leader_Update(void) {}                                                 //✅add

#endif  // VEHICLE_ROLE == ROLE_FOLLOWER                                        //✅add