#ifndef __SPACE_H__
#define __SPACE_H__

typedef struct space_state coordinate {
	int x;
	int y;
	int z;
}

coordinate curr_pos;

void setup_space_timers(void);

int calc_distance(int accel_x, int accel_y, int accel_z);
void update_coordinate(int dist);

#endif