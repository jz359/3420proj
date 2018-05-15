#include <time.h>

#include "board_accelerometer.h"
#include "board.h"

#include "utils.h"
#include "constants.h"
#include "waypoint.h"

#define PI 3.14159265

plane_state *curr_state;
volatile float dist_to_closest = 0;
volatile time_s *time_remaining = NULL;
ACCELEROMETER_STATE state;
float grav = 0;

float MAX_DISTANCE = 2000;

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
	PIT1 represents the timer to update the timer
*/
void PIT1_IRQHandler(void) {
	__disable_irq();
	int total = time_remaining->minutes * 60 + time_remaining->seconds - 1;

	time_remaining->minutes = total < 0 ? 0 : total / 60;
	time_remaining->seconds = total < 0 ? 0 : total % 60;
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1);
	PIT->CHANNEL[1].TCTRL = 3;
	__enable_irq();
}

/*
	Code to setup the PIT0
*/
void setup_led_timer(void) {
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0;

	NVIC_EnableIRQ(PIT0_IRQn);
	NVIC_EnableIRQ(PIT1_IRQn);

	PIT->CHANNEL[0].LDVAL = BLUE_LED_DELAY;
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
	PIT->CHANNEL[0].TCTRL = 3 ; // start Timer 0

	PIT->CHANNEL[1].LDVAL = DEFAULT_SYSTEM_CLOCK*2.5; // 1 s timer
	PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1);
	PIT->CHANNEL[1].TCTRL = 3 ; // start Timer 1
}

int did_exceed_bounds() {
	int x_exceed = curr_state->pos->x > 2000 || curr_state->pos->x < 0;
	int y_exceed = curr_state->pos->y > 2000 || curr_state->pos->y < 0;
	// int z_exceed = curr_state->pos->z > 2200 || curr_state->pos->z < 0;

	return (x_exceed > 0 || y_exceed > 0 || curr_state->velocity < 0);
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
	float angle = percentage*PI;

	float diff = (fabs(angle/100) > 0.01 ? angle/100 : 0)/5; //threshold val to detect change

	float updated = curr_state->heading - diff;
	float new_heading = updated < 0 ? 2 * PI + updated : updated;
	curr_state->heading = fmod(new_heading, 2 * PI);

	float heading = curr_state->heading;

	//float diff = curr_state->velocity * 0.5 * cosf(heading) * TIME_UNIT;
	curr_state->pos->x += curr_state->velocity * 0.5 * cos(heading*PI) * TIME_UNIT;
	curr_state->pos->y += curr_state->velocity * 0.5 * sin(heading*PI) * TIME_UNIT;
}

/*
	Updates the plane status with the given vector
*/
void update_plane_status(vector *state) {
	curr_state->velocity -= curr_state->velocity * 0.000005;
	calculate_pitch(state->y);
	calculate_roll(state->x);
}

/**
	Initializes the acceleration values in x_0, y_0, z_0 so we get an accurate relative acceleration
*/
void init_game_values(void) {
	//srand((unsigned int)time(NULL));
	
	Accelerometer_GetState(&state);
	x_0 = state.x;
	y_0 = state.y;
	z_0 = state.z;
	grav = ((float)state.z)/DIV_CONST;
	
	time_remaining = malloc(sizeof(time_s));
	time_remaining->minutes = 2;
	time_remaining->seconds = 0;
	
}

void init_plane_state(void) {
	curr_state = malloc(sizeof(plane_state));
	curr_state->velocity = 500;
	curr_state->heading = 0;

	curr_state->pos = malloc(sizeof(vector));
	curr_state->pos->x = BOARD_SIZE / 2;
	curr_state->pos->y = BOARD_SIZE / 2;
	curr_state->pos->z = 2000;
}

int main(){
	// init stuff
	hardware_init();
	Accelerometer_Initialize();
	LED_Initialize();
	init_game_values();
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
		
		// make printing to serial atomic
		__disable_irq();
		printf("{'v': %f, 'heading': %f, 'x': %f, 'y': %f, 'z': %f, 'nearest_wp': {'x': %f, 'y': %f, 'z': %f}, 'wp_r': %f, 'wp_hit': %d, 'time': {'m': %d, 's': %d}}\r\n",
		curr_state->velocity, curr_state->heading * 180 / PI, curr_state->pos->x, curr_state->pos->y, curr_state->pos->z,
		nearest_waypoint->near_pos->x, nearest_waypoint->near_pos->y, nearest_waypoint->near_pos->z, nearest_waypoint->near_radius, waypoints_hit, time_remaining->minutes, time_remaining->seconds);
		__enable_irq();

		//update_nearest_waypoint();

		int flag = did_hit_waypoint();

		if (flag) {
			LEDGreen_On();
			LEDBlue_Off();
			NVIC_DisableIRQ(PIT0_IRQn);
			init_waypoint();
		} else {
			LEDGreen_Off();

			dist_to_closest = is_near_waypoint();
			if (dist_to_closest > 0) {
				NVIC_EnableIRQ(PIT0_IRQn);
			} else {
				NVIC_DisableIRQ(PIT0_IRQn);
				LEDBlue_Off();
			}
		}

		/*
		// light up if nearby
		dist_to_closest = is_near_waypoint();
		if (dist_to_closest > 0) {
			// light up green if good
			if (flag) {
				LEDGreen_On();
				NVIC_DisableIRQ(PIT0_IRQn);
				init_waypoint();
			} else {
				LEDGreen_Off();
				NVIC_EnableIRQ(PIT0_IRQn);
			}
		} else {
			NVIC_DisableIRQ(PIT0_IRQn);
			LEDBlue_Off();
		}
		*/

		if (did_exceed_bounds()) {
			LEDRed_On();
			printf("CRASH");
			break;
		} else {
			LEDRed_Off();
		}

		free(relative);

		if (waypoints_hit == TOTAL_WAYPOINTS) {
			LEDGreen_On();
			printf("DONE");
			break;
		}

		for (int i = 0; i < 100000; i++);
	}
}