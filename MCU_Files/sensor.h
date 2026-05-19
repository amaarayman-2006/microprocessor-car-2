#ifndef SENSOR_H
#define SENSOR_H

void Sensor_Init(void);

long Sensor_GetFrontDistance(void);
long Sensor_GetBackDistance(void);
long Sensor_GetRightDistance(void);
long Sensor_GetLeftDistance(void);

#endif
