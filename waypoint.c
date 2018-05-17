#include "waypoint.h"

volatile waypoint* nearest_waypoint = NULL;
volatile waypoint* waypoint_head = NULL;
volatile waypoint* waypoint_tail = NULL;

volatile unsigned int waypoints_hit = 0;
volatile float angle_next_wp = 0;

/*
	Push a waypoint to the end of the waypoint queue
*/
void push_tail_waypoint(waypoint *wp) {
	if (!waypoint_head) {
		waypoint_head = wp;
		waypoint_tail = wp;
		return;
	}

	if (waypoint_tail) {
		waypoint_tail->next = wp;
	}

	waypoint_tail = wp;
	waypoint_tail->next = NULL;
}

/*
	Inits a waypoint randomly in the xy plane and within its radius
*/
void init_waypoint(void) {
	int neg = rand() % 2 == 0 ? 1 : -1;

	waypoint* wp = malloc(sizeof(waypoint));
	wp->pos = malloc(sizeof(vector));
	wp->near_pos = malloc(sizeof(vector));
	wp->pos->x = rand() % 2000;
	wp->pos->y = rand() % 2000;
	wp->pos->z = 2000;
	wp->def_radius = 100;
	wp->near_pos->x = fmod((wp->pos->x + (neg * (rand() % 100))), 2000);
	wp->near_pos->y = fmod((wp->pos->y + (neg * (rand() % 100))), 2000);
	wp->near_pos->z = wp->pos->z;
	wp->is_hit = 0;
	wp->next = NULL;

	push_tail_waypoint(wp);
}

/*
	Inits all the waypoints as specified by the game parameters
*/ 
void init_waypoints(void) {
	for (int i = 0; i < TOTAL_WAYPOINTS; i++) {
		init_waypoint();
		waypoint_tail->near_radius = 400 + i * 40;
	}
	nearest_waypoint = waypoint_head;
}

/*
	Updates to the closest nearest waypoint that exists to the plane
*/
void update_nearest_waypoint(void) {

	if (!nearest_waypoint) return; // if no waypoints nearby, exit

	float target_distance;

	if (nearest_waypoint->is_hit) {
		target_distance = 0;
	} else {
		target_distance = calc_distance(nearest_waypoint->pos, curr_state->pos);
	}

	volatile waypoint* travel = waypoint_head;
	travel->next;

	while (travel) { // find the nearest waypoint by claculating distances from all waypoints
		if (!travel->is_hit) {
			float d = calc_distance(travel->pos, curr_state->pos);
			if (d < target_distance) {
				target_distance = d;
				nearest_waypoint = travel;
			}
		}
		travel = travel->next;
	}
}

/*
	Returns if the plane is currently near the nearest waypoint, which prompts the Blue LED 
	to begin blinking (from the main loop)
*/
float is_near_waypoint(void) {
	float d = calc_distance(nearest_waypoint->pos, curr_state->pos);
	if (d <= nearest_waypoint->near_radius) {
		return d/nearest_waypoint->near_radius;
	}
	return -1.;
}

/*
	Returns if the plane has hit the waypoint, and then initializes the next waypoint in the queue
*/
int did_hit_waypoint(void) {
	if (calc_distance(nearest_waypoint->pos, curr_state->pos) <= nearest_waypoint->def_radius) {
		nearest_waypoint->is_hit = 1;
		waypoints_hit++;

		// terminate the game if all waypoints have been hit
		if (waypoints_hit == TOTAL_WAYPOINTS) {
			return 1;
		}
		
		// atomically update the time to add bonus time for hitting a waypoint
		__disable_irq();
		unsigned int curr_time = time_remaining->minutes * 60 + time_remaining->seconds + 45;
		time_remaining->minutes = curr_time / 60;
		time_remaining->seconds = curr_time % 60;
		__enable_irq();
		
		// set next waypoint
		nearest_waypoint = waypoint_head->next;
		waypoint_head = waypoint_head->next;

		int neg = rand() % 2 == 0 ? 1 : -1;
		int z = curr_state->pos->z + (neg * (rand() % 175));
		nearest_waypoint->pos->z = z < 0 ? -1 * z : z;

		return 1;
	} else {
		return 0;
	}
}
