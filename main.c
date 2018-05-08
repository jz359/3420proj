#include <fsl_device_registers.h>
#include <stdlib.h>
#include <math.h>
#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "space.h"
#include "constants.h"

#define PI 3.14159265

struct space_state {
	float x;
	float y;
	float z;
};

struct plane_state {
	float fuel;
	float velocity;
	float heading;
	vector *pos;
};

volatile vector *curr_pos;
volatile plane_state *curr_state;

/*
	PIT0 represents the timer to continually poll the LED
*/
void PIT0_IRQHandler(void) {
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

/*
	Calculates the distance based on the accelerations at a given time
*/
float calc_distance(vector *state) {
	float x = cvt_g_to_mm(state->x);
	float y = cvt_g_to_mm(state->y);
	float z = cvt_g_to_mm(state->z);
	return sqrt(x * x + y * y + z * z); // do this with the closest goal point
}

void calculate_pitch(float y) {
	float percentage = y/grav;
	curr_state->velocity -= percentage * 0.05; 
	curr_state->pos->z +=  curr_state->velocity * 0.5 * percentage * TIME_UNIT;
}

void calculate_roll(float x) {
	float percentage = x/grav;
	float angle = -percentage*PI;
	
	float v = curr_state->velocity;
	float rad = (v*v)/(tanf(angle)*grav*1000);
	
	curr_state->heading = (v*sinf(angle))/(rad*rad);

	float heading = curr_state->heading;
	
	float diff = curr_state->velocity * 0.5 * cosf(heading) * TIME_UNIT;
	curr_state->pos->x += curr_state->velocity * 0.5 * cos(heading*PI) * TIME_UNIT;
	curr_state->pos->y += curr_state->velocity * 0.5 * sin(heading*PI) * TIME_UNIT;

	printf("roll: %f, heading: %f diff: %f x: %f, y: %f \r", percentage, heading, rad, curr_state->pos->x, curr_state->pos->y);
}

/*
	Updates the plane status with the given vector
*/
void update_plane_status(vector *state) {
	// vector new_vec = get_vectorized(state);
	curr_state->fuel -= FUEL_LOSS;
	calculate_pitch(state->y);
	calculate_roll(state->x);
}

ACCELEROMETER_STATE state;
float grav = 0;

/**
	Initializes the acceleration values in x_0, y_0, z_0 so we get an accurate relative acceleration
*/
void init_accel_values(void) {
	Accelerometer_GetState(&state);
	x_0 = state.x;
	y_0 = state.y;
	z_0 = state.z;
	grav = ((float)state.z)/DIV_CONST;
}

void init_plane_state(void) {
	curr_state = malloc(sizeof(plane_state));
	curr_state->fuel = 100;
	curr_state->velocity = 100;
	curr_state->heading = 0;
	
	curr_state->pos = malloc(sizeof(vector));
	curr_state->pos->x = 0;
	curr_state->pos->y = 0;
	curr_state->pos->z = 20000;
}

int main(){
	curr_pos = malloc(sizeof(vector));
	curr_pos->x = 0;
	curr_pos->y = 0;
	curr_pos->z = 0;
	
	// init stuff 
	hardware_init();
	Accelerometer_Initialize();
	init_accel_values();
	init_plane_state();
	//setup_led_timer();
	
	// continuously poll the accelerometer
	int j = 0;
	while(1) {
		j++;
		Accelerometer_GetState(&state);
		
		// calculate the relative accelerations
		float diff_x = ((float)(state.x - x_0))/DIV_CONST;
		float diff_y = ((float)(state.y - y_0))/DIV_CONST;
		float diff_z = ((float)(state.z - z_0))/DIV_CONST;

		vector *relative = malloc(sizeof(vector));
		relative->x = diff_x;
		relative->y = diff_y;
		relative->z = diff_z;
		update_plane_status(relative);

		if (j % 1000 == 0) {
			//printf("speed: %f, altitude: %f x: %f, y: %f \r", curr_state -> velocity, curr_state->pos->z, curr_state->pos->x, curr_state->pos->y);
//			printf("relative: %f, %f, %f \r\n", relative->x,relative->y,relative->z);
//			printf("state: %d, %d, %d \r\n", state.x,state.y,state.z);
//			printf("stationary: %d, %d, %d \r\n", x_0,y_0,z_0);
//			debug_printf("----------------------\r\n");
		}
		free(relative);
		for (int i = 0; i < 100000; i++);
	}
}

