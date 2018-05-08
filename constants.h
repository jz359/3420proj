#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__
	
#include <fsl_device_registers.h>

// stationary accelerations
int x_0 = 0;
int y_0 = -40;
int z_0 = 990;

unsigned int ACCEL_THRESHOLD = 40;
unsigned int BOARD_SIZE = 100;
unsigned int ACCEL_POLL_TIME_INT = DEFAULT_SYSTEM_CLOCK / 100;
unsigned int TIME_INT_MS = 10;
unsigned int GOALS = 1;

#endif