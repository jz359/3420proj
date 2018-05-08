#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__
	
#include <fsl_device_registers.h>

// stationary accelerations
int x_0;
int y_0;
int z_0;

float grav;

unsigned int BOARD_SIZE = 100;
unsigned int ACCEL_POLL_TIME_INT = DEFAULT_SYSTEM_CLOCK / 100;
double TIME_INT_S = 0.01;
unsigned int TOTAL_WAYPOINTS = 1;
float DIV_CONST = 1000;
float FUEL_LOSS = 0.0001;
float TIME_UNIT = 0.01;

#endif