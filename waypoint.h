#include <fsl_device_registers.h>
#include <stdlib.h>
#include <math.h>
#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "space.h"
#include "constants.h"

#define PI 3.14159265

typedef struct waypoint_type {
	vector* pos;
	float def_radius;
	float near_radius;
	unsigned int is_hit;

	waypoint* next;
} waypoint;

// pointer to the nearest waypoint; constantly update
volatile extern waypoint* nearest_waypoint = NULL;
volatile extern waypoint* waypoint_head = NULL;
volatile extern waypoint* waypoint_tail = NULL;

volatile extern unsigned int waypoints_hit = 0;

/*
initialize a waypoint randomly and add it to the list
*/
void init_waypoint(void);
void update_nearest_waypoint(void);
bool is_near_waypoint(void);
bool did_hit_waypoint(void);
// void hit_waypoint(waypoint* wp);
