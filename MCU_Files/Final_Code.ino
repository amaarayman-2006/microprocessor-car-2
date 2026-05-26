#include "Motor.h"
#include "Bluetooth.h"
#include "Battery.h"
#include "Sensor.h"
#include "Modes.h"
#include "Config.h"

#define LED_PIN A5

void setup()
{
  Motor_Init();
  Bluetooth_Init();
  Battery_init();
  Sensor_Init();
  Modes_Init();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Bluetooth_Send("System Started");
}

void loop()
{
  char Command = Bluetooth_ReadCommand();

  if (Command != '\0')
  {
    Modes_HandleCommand(Command);
  }

  Modes_Update();

  // ---------- Battery + LED ----------
  int adc = analogRead(BatteryPin);
  float pinVolt = (adc * 5.0) / 1023.0;
  float batteryVolt = pinVolt * 2.98;

  if (batteryVolt <= 11)
  {
    digitalWrite(LED_PIN, HIGH);   // LOW battery
  }
  else
  {
    digitalWrite(LED_PIN, LOW);    // Battery OK
  }

  Battery_SendPercentage();
}