#ifndef __SPACE_H__
#define __SPACE_H__

typedef struct space_state {
	float x;
	float y;
	float z;
} vector;

// struct space_state;
// typedef struct space_state vector;

struct plane_state;
typedef struct plane_state plane_state;

// volatile extern vector *curr_pos;
volatile extern plane_state *curr_state;

void setup_led_timer(void);

float calc_distance(vector *state);
void update_vector(vector *state);

#endif