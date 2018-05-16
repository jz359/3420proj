#include "plane.h"

void calculate_pitch(float y) {
    float percentage = y/grav;
    curr_state->velocity -= percentage * 0.1f;
    curr_state->pos->z +=  curr_state->velocity * 0.5f * percentage * TIME_UNIT;
}

void calculate_roll(float x) {
    float percentage = x/grav;
    float angle = percentage*PI;

    float diff = (fabs(angle/100) > 0.01f ? angle/100 : 0); //threshold val to detect change

    float updated = curr_state->heading - diff;
    float new_heading = updated < 0 ? 2 * PI + updated : updated;
    curr_state->heading = fmod(new_heading, 2 * PI);

    float heading = curr_state->heading;

    curr_state->pos->x += curr_state->velocity * 0.5f * cos(heading) * TIME_UNIT;
    curr_state->pos->y += curr_state->velocity * 0.5f * sin(heading) * TIME_UNIT;
}

/*
    Updates the plane status with the given vector
*/
void update_plane_status(vector *state) {
    curr_state->velocity -= curr_state->velocity * 0.000005f;
    calculate_pitch(state->y);
    calculate_roll(state->x);
}