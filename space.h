#ifndef __SPACE_H__
#define __SPACE_H__

typedef struct space_state {
	float x;
	float y;
	float z;
} vector;

typedef struct plane_state {
	float velocity;
	float heading;
	vector *pos;
} plane_state;

volatile extern plane_state *curr_state;

void setup_led_timer(void);
int did_exceed_bounds(void);
float calc_distance(vector *state, vector *state2);
void calculate_pitch(float y);
void calculate_roll(float x);
void update_plane_status(vector *state);

#endif
