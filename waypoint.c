#include "waypoint.h"

// helpers
waypoint * pop_front_waypoint() {
	if (!waypoint_head) return NULL;
	waypoint *wp = waypoint_head;
	waypoint_head = wp->next;
	if (waypoint_tail == wp) {
		waypoint_tail = NULL;
	}

	wp->next = NULL;
	return wp;
}

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
	// TODO randomize the waypoint position
	waypoint* wp = malloc(sizeof(waypoint));
	wp->pos->x = 0;
	wp->pos->y = 0;
	wp->pos->z = 0;
	wp->def_radius = 3;
	wp->near_radius = 5;
	wp->is_hit = 0;
	wp->next = NULL;

	push_tail_waypoint(wp);
}

void update_nearest_waypoint(void) {

	if (!nearest_waypoint) return;

	float target_distance;

	if (nearest_waypoint->is_hit) {
		target_distance = 0;
	} else {
		target_distance = calc_distance(nearest_waypoint->pos, curr_state->pos);
	}

	waypoint* travel = waypoint_head;
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

bool is_near_waypoint(void) {
	float d = calc_distance(nearest_waypoint->pos, curr_state->pos);
	return (d <= nearest_waypoint->near_radius);
}

bool did_hit_waypoint(void) {
	if (calc_distance(nearest_waypoint->pos, curr_state->pos) <= nearest_waypoint->def_radius) {
		nearest_waypoint->is_hit = 1;
		waypoints_hit++;
		return true;
	} else {
		return false;
	}
}

/*
void hit_waypoint(waypoint* wp) {
	wp->is_hit = 1;
	waypoints_hit += 1;
}*/
