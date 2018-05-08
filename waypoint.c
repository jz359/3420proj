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
	wp->is_hit = false;
	wp->next = NULL;

	push_tail_waypoint(wp);
}

void update_nearest_waypoint(void) {

}

bool is_near_waypoint(void) {

}

void hit_waypoint(waypoint* wp) {

}
