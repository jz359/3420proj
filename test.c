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

// initializes the plane's position and velocity on the board
void init_plane_state(void) {
	curr_state = malloc(sizeof(plane_state));
	curr_state->velocity = 400;
	curr_state->heading = 0;

	curr_state->pos = malloc(sizeof(vector));
	curr_state->pos->x = BOARD_SIZE / 2;
	curr_state->pos->y = BOARD_SIZE / 2;
	curr_state->pos->z = 2000;
}

// handler for calibration and increasing speed
void PORTC_IRQHandler(void){
	if (!calibrated) calibrated = 1;
	else {
		curr_state->velocity+=SPEED_INC;
	}
	PORTC->PCR[6] |= PORT_PCR_ISF(1);
}

// handler for decreasing speed
void PORTA_IRQHandler(void) {
	curr_state->velocity-=SPEED_INC;
	PORTA->PCR[4] |= PORT_PCR_ISF(1);
}

// enables SW2 and SW3 interrupts for PORTC and PORTA, respectively
void setup_switches(void){
	// NOTE: have to do FOPT EQU 0x000 in startup_MK64F12.s for SW3 to work
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

// prints the necessary data to the serial output
void print_to_serial(void) {
		__disable_irq();
		printf("{'v': %f, 'heading': %f, 'x': %f, 'y': %f, 'z': %f, 'nearest_wp': {'x': %f, 'y': %f, 'z': %f}, 'wp_r': %f, 'wp_hit': %d, 'time': {'m': %d, 's': %d}}\r\n",
		curr_state->velocity, curr_state->heading * 180 / PI, curr_state->pos->x, curr_state->pos->y, curr_state->pos->z,
		nearest_waypoint->near_pos->x, nearest_waypoint->near_pos->y, nearest_waypoint->near_pos->z, nearest_waypoint->near_radius, waypoints_hit, time_remaining->minutes, time_remaining->seconds);
		__enable_irq();
}

int main(){
	// init stuff
	hardware_init();
	Accelerometer_Initialize();
	LED_Initialize();
	init_game_values();
	init_plane_state();
	// setup_led_timer();
	// setup_switches();

	TOTAL_WAYPOINTS = 2;

	init_waypoints();

	//print_to_serial(); // print initial state to serial
	//while (!calibrated); // wait to start game and calibrate

	// test boundary conditions
	curr_state->pos->x = 2001;
	printf("Bound check should be 1: %d\r\n", did_exceed_bounds());
	curr_state->pos->x = BOARD_SIZE /2;

	curr_state->velocity = 1;
	printf("Bound check should be 1: %d\r\n", did_exceed_bounds());
	curr_state->velocity = 400;

	time_remaining->minutes = 0;
	time_remaining->seconds = 0;
	printf("Bound check should be 1: %d\r\n", did_exceed_bounds());
	time_remaining->minutes = 2;
	
	// test waypoint hit logic
	nearest_waypoint->pos->x = BOARD_SIZE / 2;
	nearest_waypoint->pos->y = BOARD_SIZE / 2;
	nearest_waypoint->pos->z = 2000;
	int flag = did_hit_waypoint();
	printf("Waypoint hit should be 1: %d, Waypoints hit should be 1: %d, Time remaining should be 2min45sec: %d:%d\r\n", 
		flag, waypoints_hit, time_remaining->minutes, time_remaining->seconds);

	// test end-game logic
	nearest_waypoint->pos->x = BOARD_SIZE / 2;
	nearest_waypoint->pos->y = BOARD_SIZE / 2;
	nearest_waypoint->pos->z = 2000;
	flag = did_hit_waypoint();
	printf("Waypoint hit should be 1: %d, Waypoints hit should be 2: %d, Time remaining should be 2min45sec: %d:%d\r\n", 
		flag, waypoints_hit, time_remaining->minutes, time_remaining->seconds);
}
