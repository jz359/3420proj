#include "waypoint.h"

// helpers
/*
waypoint * pop_front_waypoint() {
	if (!waypoint_head) return NULL;
	volatile waypoint *wp = waypoint_head;
	waypoint_head = wp->next;
	if (waypoint_tail == wp) {
		waypoint_tail = NULL;
	}

	wp->next = NULL;
	return wp;
}*/

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
		wp->prev = waypoint_tail;
	}

	waypoint_tail = wp;
	waypoint_tail->next = NULL;
}
// /helpers

void init_waypoint(void) {
	// TODO randomize the waypoint position
	waypoint* wp = malloc(sizeof(waypoint));
	wp->pos = malloc(sizeof(vector));
	wp->pos->x = curr_state->pos->x + 400;
	wp->pos->y = curr_state->pos->y;
	wp->pos->z = curr_state->pos->z;
	wp->def_radius = 200;
	wp->near_radius = 400;
	wp->is_hit = 0;
	wp->next = NULL;
	wp->prev = NULL;

	push_tail_waypoint(wp);
	
	if (!nearest_waypoint) {
		nearest_waypoint = wp;
	}
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
}

float vector_to_norm(vector* v) {
		return (v->x * v->x + v->y*v->y + v->z*v->z);
}

float dot_product(vector* v1, vector* v2) {
	return (v1->x*v2->x + v1->y*v2->y + v1->z*v2->z);
} */

void get_angle_nearest_waypoint(void) {
	float x = nearest_waypoint->pos->x - curr_state->pos->x;
	float y = nearest_waypoint->pos->y - curr_state->pos->y;
	
	angle_next_wp = atan2(y, x) * 180.0 / PI - curr_state->heading;
	//angle_next_wp = atan2(y, x) * 180.0 / PI;
	/*
	if (angle_next_wp > 180) {
		angle_next_wp = 360 - angle_next_wp;
	} else if (angle_next_wp < -180) {
		angle_next_wp = 360 + angle_next_wp;
	}*/
}

void free_waypoint(void) {
	
	waypoint* temp = nearest_waypoint;
	
	// reset the nearest waypoint to a temporary value
	if(temp->next) nearest_waypoint = temp->next;
	else if (temp->prev) nearest_waypoint = temp->prev;
	else nearest_waypoint = NULL;
	
	if (temp->prev) {
		temp->prev->next = temp->next;
	}

	if(temp->next) {
		temp->next->prev = temp->prev;
	}
	
	free(temp);
}
