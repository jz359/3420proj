#ifndef __SPACE_H__
#define __SPACE_H__

#include <stdlib.h>

struct space_state;
typedef struct space_state coordinate;

extern coordinate *curr_pos;

void setup_space_timers(void);

int calc_distance(int accel_x, int accel_y, int accel_z);
void update_coordinate(int dist);

#endif