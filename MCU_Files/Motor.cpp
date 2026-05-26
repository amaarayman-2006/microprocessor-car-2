#include <Arduino.h>
#include "Config.h"
#include "Motor.h"

static int CurrentSpeed = 0;

void Motor_Init(void)
{
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LeftSpeed, OUTPUT);
  pinMode(RightSpeed, OUTPUT);

  Motor_SetSpeed(MinPWM);
  Motor_Stop();
}

void Motor_SetSpeed(int Speed)
{
  if (Speed < MinPWM)
  {
    Speed = MinPWM;
  }

  if (Speed > MaxPWM)
  {
    Speed = MaxPWM;
  }

  CurrentSpeed = Speed;
}

void Motor_Stop(void)
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(LeftSpeed, MinPWM);
  analogWrite(RightSpeed, MinPWM);
}

void Motor_Brake(void)
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);

  analogWrite(LeftSpeed, MaxPWM);
  analogWrite(RightSpeed, MaxPWM);
}

void Motor_Forward(void)
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(LeftSpeed, CurrentSpeed);
  analogWrite(RightSpeed, CurrentSpeed);
}

void Motor_Backward(void)
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(LeftSpeed, CurrentSpeed);
  analogWrite(RightSpeed, CurrentSpeed);
}

void Motor_Left(void)
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(LeftSpeed, CurrentSpeed);
  analogWrite(RightSpeed, CurrentSpeed);
}

void Motor_Right(void)
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(LeftSpeed, CurrentSpeed);
  analogWrite(RightSpeed, CurrentSpeed);
}

void Motor_ForwardLeft(void)
{
  int TurnSpeed = CurrentSpeed * TurnRatio;

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(LeftSpeed, TurnSpeed);
  analogWrite(RightSpeed, CurrentSpeed);
}

void Motor_ForwardRight(void)
{
  int TurnSpeed = CurrentSpeed * TurnRatio;

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  analogWrite(LeftSpeed, CurrentSpeed);
  analogWrite(RightSpeed, TurnSpeed);
}

void Motor_BackwardLeft(void)
{
  int TurnSpeed = CurrentSpeed * TurnRatio;

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(LeftSpeed, TurnSpeed);
  analogWrite(RightSpeed, CurrentSpeed);
}

void Motor_BackwardRight(void)
{
  int TurnSpeed = CurrentSpeed * TurnRatio;

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(LeftSpeed, CurrentSpeed);
  analogWrite(RightSpeed, TurnSpeed);
}
