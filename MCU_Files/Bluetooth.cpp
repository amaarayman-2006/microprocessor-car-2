#include "Bluetooth.h"
#include "Config.h"
#include <Arduino.h>

void Bluetooth_Init(void)
{
  Serial.begin(BT_BAUD_RATE);
}

char Bluetooth_ReadCommand(void)
{
  if (Serial.available() > 0)
  {
    return Serial.read();
  }

  return '\0';
}

void Bluetooth_Send(const char* message)
{
  Serial.println(message);
}
