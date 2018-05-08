#ifndef __SPACE_H__
#define __SPACE_H__

struct space_state;
typedef struct space_state coordinate;

volatile extern coordinate *curr_pos;

void setup_led_timer(void);

float calc_distance(coordinate *state);
void update_coordinate(coordinate *state);

#endif