#ifndef __SPACE_H__
#define __SPACE_H__

struct space_state;
typedef struct space_state coordinate;

extern coordinate *curr_pos;

void setup_space_timers(void);

double calc_distance(coordinate state);
void update_coordinate(coordinate state);

#endif