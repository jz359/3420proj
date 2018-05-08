#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__
	
#include <fsl_device_registers.h>

// stationary accelerations
extern int x_0;
extern int y_0;
extern int z_0;

extern float grav;

extern unsigned int BOARD_SIZE;
extern unsigned int ACCEL_POLL_TIME_INT;
extern double TIME_INT_S;
extern unsigned int TOTAL_WAYPOINTS;
extern float DIV_CONST;
extern float FUEL_LOSS;
extern float TIME_UNIT;

#endif