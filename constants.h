#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__
	
#include <fsl_device_registers.h>

// stationary accelerations
int x_stationary = 0;
int y_stationary = 0;
int z_stationary = 0;

unsigned int ACCEL_THRESHOLD = 40;
unsigned int BOARD_SIZE = 100;
unsigned int ACCEL_POLL_TIME_INT = DEFAULT_SYSTEM_CLOCK / 100;
double TIME_INT_S = 0.01;
unsigned int GOALS = 1;

#endif