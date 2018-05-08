#include <fsl_device_registers.h>
#include <stdlib.h>
#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "space.h"
#include "constants.h"

struct space_state {
	double x;
	double y;
	double z;
};


coordinate *curr_pos;

/*
	PIT0 represents the timer to continually poll the LED
*/
void PIT0_IRQHandler(void) {
	debug_printf("kek \r\n");
}

/*
	Code to setup the PIT0	
*/
void setup_led_timer(void) {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0;

	NVIC_EnableIRQ(PIT0_IRQn);
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK;
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	PIT->CHANNEL[0].TCTRL = 3 ; // start Timer 0
}

double cvt_g_to_mm (int gforce) {
	return 0.5 * 9.80665 * (double) gforce * TIME_INT_S * TIME_INT_S;
}

coordinate get_vectorized(coordinate state) {
	coordinate vector = {
		cvt_g_to_mm(state.x),
		cvt_g_to_mm(state.y),
		cvt_g_to_mm(state.z)
	};
	return vector;
}

/*
	Calculates the distance based on the accelerations at a given time
*/
double calc_distance(coordinate state) {
	double x = cvt_g_to_mm(state.x);
	double y = cvt_g_to_mm(state.y);
	double z = cvt_g_to_mm(state.z);
	return sqrt(x * x + y * y + z * z); // do this with the closest goal point
}

/*
	Updates the coordinate curr_pos to the current locations.
*/
void update_coordinate(coordinate state) {
	coordinate new_vec = get_vectorized(state);
	curr_pos->x += new_vec.x;
	curr_pos->y += new_vec.y;
	curr_pos->z += new_vec.z;
}

ACCELEROMETER_STATE state;
int x_0;
int y_0;
int z_0;

void init_accel_values(void) {
	Accelerometer_GetState(&state);
	x_0 = state.x;
	y_0 = state.y;
	z_0 = state.z;

	x_stationary = state.x;
	y_stationary = state.y;
	z_stationary = state.z;	
}

int main(){
	curr_pos = malloc(sizeof(coordinate));
	curr_pos->x = 0;
	curr_pos->y = 0;
	curr_pos->z = 0;
	
	hardware_init();
	Accelerometer_Initialize();
	init_accel_values();
	while(1) {
		Accelerometer_GetState(&state);
		if (abs(state.x - x_0) > ACCEL_THRESHOLD 
			|| abs(state.y - y_0) > ACCEL_THRESHOLD 
			|| abs(state.z - z_0) > ACCEL_THRESHOLD) {
			debug_printf("changed!\r\n");
				
			coordinate *relative = malloc(sizeof(coordinate));
			relative->x = state.x - x_stationary;
			relative->y = state.y - y_stationary;
			relative->z = state.z - z_stationary;
			debug_printf("state: %5d, %5d, %5d \r\n", state.x,state.y,state.z);
			debug_printf("stationary: %5d, %5d, %5d \r\n", x_stationary,y_stationary,z_stationary);
			debug_printf("relative: %5d, %5d, %5d \r\n", relative->x,relative->y,relative->z);
			debug_printf("----------------------\r\n");
			free(relative);
			//update_coordinate(relative);
			//debug_printf("%5d, %5d, %5d \r\n", curr_pos->x,curr_pos->y,curr_pos->z);
			};
		x_0 = state.x;
		y_0 = state.y;
		z_0 = state.z;
		for (int i = 0; i < 10000; i++);
	}
}

