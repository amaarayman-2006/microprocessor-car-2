#ifndef MOTOR_H
#define MOTOR_H

void Motor_Init(void);
void Motor_SetSpeed(int Speed);

void Motor_Stop(void);
void Motor_Brake(void);

void Motor_Forward(void);
void Motor_Backward(void);

void Motor_Left(void);
void Motor_Right(void);

void Motor_ForwardLeft(void);
void Motor_ForwardRight(void);

void Motor_BackwardLeft(void);
void Motor_BackwardRight(void);

#endif
