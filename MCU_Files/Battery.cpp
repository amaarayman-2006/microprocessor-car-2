#include <Arduino.h>
#include "Config.h"
#include "Battery.h"
#include "Bluetooth.h"

static unsigned long LastBatterySendTime = 0;
static unsigned long BatterySendInterval = 3000;

void Battery_init(void)
{
  pinMode(BatteryPin, INPUT);
}

int Battery_GetPercentage(void)
{
  int adc = analogRead(BatteryPin);

  float pinVolt = (adc * 5.0) / 1023.0;

  float batteryVolt = pinVolt * ((R1 + R2) / R2);

  int percentage = (batteryVolt / BatteryFull) * 100;

  if (percentage < 0)
  {
    percentage = 0;
  }

  if (percentage > 100)
  {
    percentage = 100;
  }

  return percentage;
}

void Battery_SendPercentage(void)
{
  unsigned long CurrentTime = millis();

  if (CurrentTime - LastBatterySendTime >= BatterySendInterval)
  {
    LastBatterySendTime = CurrentTime;

    int Percentage = Battery_GetPercentage();

    char Message[15];

    sprintf(Message, "%d%%", Percentage);

    Bluetooth_Send(Message);
  }
}
