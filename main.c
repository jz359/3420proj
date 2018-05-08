#include <fsl_device_registers.h>
#include <stdlib.h>
#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "space.h"
#include "constants.h"

struct space_state {
	float x;
	float y;
	float z;
};


volatile coordinate *curr_pos;

/*
	PIT0 represents the timer to continually poll the LED
*/
void PIT0_IRQHandler(void) {
		printf("%f, %f, %f \r", curr_pos->x,curr_pos->y,curr_pos->z);
		PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
		PIT->CHANNEL[0].TCTRL = 3 ; // start Timer 0
}

/*
	Code to setup the PIT0	
*/
void setup_led_timer(void) {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0;

	NVIC_EnableIRQ(PIT0_IRQn);
	PIT->CHANNEL[0].LDVAL = DEFAULT_SYSTEM_CLOCK * 2;
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	PIT->CHANNEL[0].TCTRL = 3 ; // start Timer 0
}

float cvt_g_to_mm (float gforce) {
	float mm = 0.5 * 9.80665 * gforce * TIME_INT_S * TIME_INT_S;
	return mm;
}

coordinate get_vectorized(coordinate *state) {
	coordinate vector = {
		cvt_g_to_mm(state->x),
		cvt_g_to_mm(state->y),
		cvt_g_to_mm(state->z)
	};
	return vector;
}

/*
	Calculates the distance based on the accelerations at a given time
*/
float calc_distance(coordinate *state) {
	float x = cvt_g_to_mm(state->x);
	float y = cvt_g_to_mm(state->y);
	float z = cvt_g_to_mm(state->z);
	return sqrt(x * x + y * y + z * z); // do this with the closest goal point
}

/*
	Updates the coordinate curr_pos to the current locations.
*/
void update_coordinate(coordinate *state) {
	coordinate new_vec = get_vectorized(state);

	curr_pos->x += new_vec.x;
	curr_pos->y += new_vec.y;
	curr_pos->z += new_vec.z;
}

ACCELEROMETER_STATE state;

/**
	Initializes the acceleration values in x_0, y_0, z_0 so we get an accurate relative acceleration
*/
void init_accel_values(void) {
	Accelerometer_GetState(&state);
	x_0 = state.x;
	y_0 = state.y;
	z_0 = state.z;
}

int main(){
	curr_pos = malloc(sizeof(coordinate));
	curr_pos->x = 0;
	curr_pos->y = 0;
	curr_pos->z = 0;
	
	// init stuff 
	hardware_init();
	Accelerometer_Initialize();
	init_accel_values();
	setup_led_timer();
	
	// continuously poll the accelerometer
	while(1) {
		Accelerometer_GetState(&state);
		
		// calculate the relative accelerations
		int diff_x = state.x - x_0;
		int diff_y = state.y - y_0;
		int diff_z = state.z - z_0;

		coordinate *relative = malloc(sizeof(coordinate));
		relative->x = diff_x;
		relative->y = diff_y;
		relative->z = diff_z;
		update_coordinate(relative);
		free(relative);
		
		x_0 = state.x;
		y_0 = state.y;
		z_0 = state.z;
		for (int i = 0; i < 10000; i++);
	}
}
