#include <Arduino.h>
#include "Config.h"
#include "Sensor.h"

static long Sensor_ReadDistance(int TrigPin, int EchoPin)
{
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);

  long Duration = pulseIn(EchoPin, HIGH, ULTRASONIC_TIMEOUT);

  if (Duration == 0)
  {
    return -1;
  }

  long Distance = Duration / 58;
  return Distance;
}

void Sensor_Init(void)
{
  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(BACK_TRIG, OUTPUT);
  pinMode(BACK_ECHO, INPUT);

  pinMode(RIGHT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);

  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);

  digitalWrite(FRONT_TRIG, LOW);
  digitalWrite(BACK_TRIG, LOW);
  digitalWrite(RIGHT_TRIG, LOW);
  digitalWrite(LEFT_TRIG, LOW);
}

long Sensor_GetFrontDistance(void)
{
  return Sensor_ReadDistance(FRONT_TRIG, FRONT_ECHO);
}

long Sensor_GetBackDistance(void)
{
  return Sensor_ReadDistance(BACK_TRIG, BACK_ECHO);
}

long Sensor_GetRightDistance(void)
{
  return Sensor_ReadDistance(RIGHT_TRIG, RIGHT_ECHO);
}

long Sensor_GetLeftDistance(void)
{
  return Sensor_ReadDistance(LEFT_TRIG, LEFT_ECHO);
}