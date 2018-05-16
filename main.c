#include "board_accelerometer.h"
#include "board.h"

#include "utils.h"
#include "constants.h"
#include "waypoint.h"
#include <stdlib.h>
#include <stdio.h>

volatile plane_state *curr_state;
volatile float dist_to_closest = 0;
volatile time_s *time_remaining = NULL;
ACCELEROMETER_STATE state;
float grav = 0;
float MAX_DISTANCE = 2000;

volatile int calibrated = 0;

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


/**
	Initializes the acceleration values in x_0, y_0, z_0 so we get an accurate relative acceleration
*/
void init_game_values(void) {
	Accelerometer_GetState(&state);
	x_0 = state.x;
	y_0 = state.y;
	z_0 = state.z;
	srand(state.z);
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

void PORTC_IRQHandler(void){
	if (!calibrated) calibrated = 1;
	else {
		curr_state->velocity+=SPEED_INC;
	}
	PORTC->PCR[6] |= PORT_PCR_ISF(1);
}

void PORTA_IRQHandler(void) {
	curr_state->velocity-=SPEED_INC;
	PORTA->PCR[4] |= PORT_PCR_ISF(1);
}

void setup_switches(void){
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	PORTC->PCR[6] = PORT_PCR_MUX(001);
	PTC->PDDR &= ~(1<<6);
	PORTC->PCR[6] |= PORT_PCR_IRQC(0xA);
	PORTC->PCR[6] |= PORT_PCR_PE(1);
	PORTC->PCR[6] |= PORT_PCR_PS(1);
	NVIC_EnableIRQ(PORTC_IRQn);

	PORTA->PCR[4] |= PORT_PCR_MUX(001); 
	PTA->PDDR &= ~(1<<4);
	PORTA->PCR[4] |= PORT_PCR_IRQC(0xA);
	PORTA->PCR[4] |= PORT_PCR_PE(1);
	PORTA->PCR[4] |= PORT_PCR_PS(1);
	NVIC_EnableIRQ(PORTA_IRQn); 
}

int main(){
	// init stuff
	hardware_init();
	Accelerometer_Initialize();
	LED_Initialize();
	init_game_values();
	init_plane_state();

	setup_led_timer();
	setup_switches();
	init_waypoints();

	while (!calibrated); // wait to start game
	// continuously poll the accelerometer
	while(1) {
		Accelerometer_GetState(&state);

		// calculate the relative accelerations
		vector *relative = malloc(sizeof(vector));
		relative->x = ((float)(state.x - x_0))/DIV_CONST;
		relative->y = ((float)(state.y - y_0))/DIV_CONST;
		relative->z = ((float)(state.z - z_0))/DIV_CONST;
		
		// update the plane's status 
		update_plane_status(relative);
		free(relative);

		if (did_hit_waypoint()) {
			LEDGreen_On();
			LEDBlue_Off();
			NVIC_DisableIRQ(PIT0_IRQn);
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

		if (did_exceed_bounds()) {
			LEDRed_On();
			printf("CRASH");
			break;
		}

		if (waypoints_hit == TOTAL_WAYPOINTS) {
			LEDGreen_On();
			printf("DONE");
			break;
		}

		// make printing to serial atomic
		__disable_irq();
		printf("{'v': %f, 'heading': %f, 'x': %f, 'y': %f, 'z': %f, 'nearest_wp': {'x': %f, 'y': %f, 'z': %f}, 'wp_r': %f, 'wp_hit': %d, 'time': {'m': %d, 's': %d}}\r\n",
		curr_state->velocity, curr_state->heading * 180 / PI, curr_state->pos->x, curr_state->pos->y, curr_state->pos->z,
		nearest_waypoint->near_pos->x, nearest_waypoint->near_pos->y, nearest_waypoint->near_pos->z, nearest_waypoint->near_radius, waypoints_hit, time_remaining->minutes, time_remaining->seconds);
		__enable_irq();

		for (int i = 0; i < 100000; i++);
	}
}
