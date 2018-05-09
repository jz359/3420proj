#include <fsl_device_registers.h>
#include <stdlib.h>
#include <math.h>
#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "utils.h"
#include "constants.h"
#include "waypoint.h"
#include "utils.h"

#define PI 3.14159265

// volatile vector *curr_pos;
volatile plane_state *curr_state;

volatile float dist_to_closest = 0;
/*
	PIT0 represents the timer to continually poll the LED
*/
void PIT0_IRQHandler(void) {
	LEDBlue_Toggle();
	PIT->CHANNEL[0].LDVAL = calc_flash_delay(dist_to_closest);
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	PIT->CHANNEL[0].TCTRL = 3;
}

/*
	Code to setup the PIT0	
*/
void setup_led_timer(void) {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0;

	PIT->CHANNEL[0].LDVAL = BLUE_LED_DELAY;
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
float calc_distance(vector *state, vector *state2) {
	float x_d = state->x - state2->x;
	float y_d = state->y - state2->y;
	float z_d = state->z - state2->z;
	return sqrt(x_d * x_d + y_d * y_d + z_d * z_d);
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
	
	float diff = (fabs(angle/100) > 0.01 ? angle/100 : 0)/5; //threshold val to detect change
	curr_state->heading = fmod(curr_state->heading - diff, 2 * PI); 

	float heading = curr_state->heading;
	
	//float diff = curr_state->velocity * 0.5 * cosf(heading) * TIME_UNIT;
	curr_state->pos->x += curr_state->velocity * 0.5 * cos(heading*PI) * TIME_UNIT;
	curr_state->pos->y += curr_state->velocity * 0.5 * sin(heading*PI) * TIME_UNIT;

	printf("roll: %f, heading: %f diff: %f x: %f, y: %f \r", percentage, heading * 180 / PI, diff, curr_state->pos->x, curr_state->pos->y);
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
	/*
	curr_pos = malloc(sizeof(vector));
	curr_pos->x = 0;
	curr_pos->y = 0;
	curr_pos->z = 0;
	*/
	
	// init stuff 
	hardware_init();
	Accelerometer_Initialize();
	LED_Initialize();
	init_accel_values();
	init_plane_state();

	setup_led_timer();
	init_waypoint();
	
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
		
		update_nearest_waypoint();
		
		int flag = did_hit_waypoint();
		
		// light up if nearby
		dist_to_closest = is_near_waypoint();
		if (dist_to_closest > 0) {
			NVIC_EnableIRQ(PIT0_IRQn);
		} else {
			NVIC_DisableIRQ(PIT0_IRQn);
			LEDBlue_Off();
		}
		
		// light up green if good
		if (is_on_waypoint()) {
			LEDGreen_On();
			NVIC_DisableIRQ(PIT0_IRQn);
		} else {
			LEDGreen_Off();
			NVIC_EnableIRQ(PIT0_IRQn);
		}

		free(relative);
		for (int i = 0; i < 100000; i++);
	}
}

