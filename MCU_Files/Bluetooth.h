#ifndef BLUETOOTH_H
#define BLUETOOTH_H

void Bluetooth_Init(void);

char Bluetooth_ReadCommand(void);

void Bluetooth_Send(const char* message);

#endif
