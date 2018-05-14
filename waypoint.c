<<<<<<< HEAD
#include "waypoint.h"

volatile waypoint* nearest_waypoint = NULL;
volatile waypoint* waypoint_head = NULL;
volatile waypoint* waypoint_tail = NULL;

volatile unsigned int waypoints_hit = 0;
volatile float angle_next_wp = 0;

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
// /helpers

void init_waypoint(void) {
	int neg = rand() % 2 == 0 ? 1 : -1;
	int z = curr_state->pos->z + (neg * (rand() % 100));

	waypoint* wp = malloc(sizeof(waypoint));
	wp->pos = malloc(sizeof(vector));
	wp->near_pos = malloc(sizeof(vector));
	wp->pos->x = rand() % 2000;
	wp->pos->y = rand() % 2000;
	wp->pos->z = z < 0 ? -1 * z : z % 2200;
	wp->def_radius = 75;
	wp->near_pos->x = fmod((wp->pos->x + (neg * (rand() % 100))), 2000);
	wp->near_pos->y = fmod((wp->pos->y + (neg * (rand() % 100))), 2000);
	wp->near_pos->z = wp->pos->z;
	wp->near_radius = 400;
	wp->is_hit = 0;
	wp->next = NULL;

	push_tail_waypoint(wp);

	nearest_waypoint = wp;
}

void update_nearest_waypoint(void) {

	if (!nearest_waypoint) return;

	float target_distance;

	if (nearest_waypoint->is_hit) {
		target_distance = 0;
	} else {
		target_distance = calc_distance(nearest_waypoint->pos, curr_state->pos);
	}

	volatile waypoint* travel = waypoint_head;
	travel->next;

	while (travel) {
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

float is_near_waypoint(void) {
	float d = calc_distance(nearest_waypoint->pos, curr_state->pos);
	if (d <= nearest_waypoint->near_radius) {
		return d/nearest_waypoint->near_radius;
	}
	return -1.;
}

int did_hit_waypoint(void) {
	if (calc_distance(nearest_waypoint->pos, curr_state->pos) <= nearest_waypoint->def_radius) {
		nearest_waypoint->is_hit = 1;
		waypoints_hit++;
		return 1;
	} else {
		return 0;
	}
}

/*
int is_on_waypoint(void) {
	int i = calc_distance(nearest_waypoint->pos, curr_state->pos) <= nearest_waypoint->def_radius;
	return i;
}*/