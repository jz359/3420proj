#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

//coordinate goal_coords[];

#include <fsl_device_registers.h>
#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "space.h"
#include "constants.h"

coordinate *curr_pos = NULL;

struct space_state {
	int x;
	int y;
	int z;
};

/*
	PIT0 represents the timer to continually poll the accelerations
*/
void PIT0_IRQHandler(void) {
	debug_printf("kek \r\n");
}

/*
	Code to setup the PIT0 and PIT1 timers	
*/
void setup_space_timers(void) {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0;

	NVIC_EnableIRQ(PIT0_IRQn);
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK;
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	PIT->CHANNEL[0].TCTRL = 3 ; // start Timer 0
}

/*
	Calculates the distance based on the accelerations at a given time
*/
int calc_distance(int accel_x, int accel_y, int accel_z) {
	return 0;
}

/*
	Updates the coordinate curr_pos to the current locations.
*/
void update_coordinate(int dist) {

}


ACCELEROMETER_STATE state;
int main(){
	hardware_init();
	Accelerometer_Initialize();
	//setup_space_timers();
	while(1) {
		//debug_printf("FCK whats up dog\r\n");
		Accelerometer_GetState(&state);
		if (abs(state.x - x_0) > ACCEL_THRESHOLD) {
			debug_printf("x changed! %5d \r\n", state.x);
		}
		if (abs(state.y - y_0) > ACCEL_THRESHOLD) {
			debug_printf("y changed! %5d \r\n", state.y);
		}	
		if (abs(state.z - z_0) > ACCEL_THRESHOLD) {
			debug_printf("z changed! %5d \r\n", state.z);
		}
		x_0 = state.x;
		y_0 = state.y;
		z_0 = state.z;
		for (int i = 0; i < 10000; i++);
	}
}

