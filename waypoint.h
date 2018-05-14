#include "space.h"
#include "constants.h"
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265

struct waypoint_type {
	vector* pos;
	vector* near_pos;
	float def_radius;
	float near_radius;
	unsigned int is_hit;

	struct waypoint_type* next;
};

typedef struct waypoint_type waypoint;

// pointer to the nearest waypoint; constantly update
volatile extern waypoint* nearest_waypoint;
volatile extern waypoint* waypoint_head;
volatile extern waypoint* waypoint_tail;

volatile extern unsigned int waypoints_hit;
volatile extern float angle_next_wp;

/*
initialize a waypoint randomly and add it to the list
*/
void init_waypoint(void);
void update_nearest_waypoint(void);
float is_near_waypoint(void);
int did_hit_waypoint(void);
//int is_on_waypoint(void);
void get_angle_nearest_waypoint(void);
// void hit_waypoint(waypoint* wp);
