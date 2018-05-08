#ifndef __SPACE_H__
#define __SPACE_H__

typedef struct space_state {
	float x;
	float y;
	float z;
} vector;

typedef struct plane_state {
	float fuel;
	float velocity;
	float heading;
	vector *pos;
} plane_state;


// struct space_state;
// typedef struct space_state vector;

// volatile extern vector *curr_pos;
volatile extern plane_state *curr_state;

void setup_led_timer(void);

float calc_distance(vector *state, vector* state2);
void update_vector(vector *state);

#endif