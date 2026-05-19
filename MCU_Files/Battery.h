#ifndef BATTERY_H
#define BATTERY_H

void Battery_init(void);

int Battery_GetPercentage(void);

void Battery_SendPercentage(void);

#endif
