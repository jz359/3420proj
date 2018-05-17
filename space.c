#include "space.h"
#include "constants.h"

/*
    Code to setup the PIT0
*/
void setup_led_timer(void) {
    SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
    PIT->MCR = 0;

    PIT->CHANNEL[0].LDVAL = BLUE_LED_DELAY;
    PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF(1);
    PIT->CHANNEL[0].TCTRL = 3 ; // start Timer 0

    PIT->CHANNEL[1].LDVAL = DEFAULT_SYSTEM_CLOCK*2.5; // 1 s timer
    PIT->CHANNEL[1].TFLG = PIT_TFLG_TIF(1);
    PIT->CHANNEL[1].TCTRL = 3 ; // start Timer 1
}

/*
    Return true if the plane has exceeded the bounds of the game, which will terminate the main loop
*/
int did_exceed_bounds() {
    int x_exceed = curr_state->pos->x > 2000 || curr_state->pos->x < 0;
    int y_exceed = curr_state->pos->y > 2000 || curr_state->pos->y < 0;
    int time_exceeded = !time_remaining->minutes && !time_remaining->seconds;

    return (x_exceed > 0 || y_exceed > 0 || curr_state->pos->z <= 0 || curr_state->velocity < 50 || time_exceeded);
}

/*
    Calculates the distance based on the accelerations at a given time
*/
float calc_distance(vector *state, vector *state2) {
    float x_d = state->x - state2->x;
    float y_d = state->y - state2->y;
    float z_d = state->z - state2->z;
    return sqrt(x_d * x_d + y_d * y_d + z_d * z_d);
}

/*
    Takes the current relative pitch from the accelerometer data and updates the speed and position of the plane
*/
void calculate_pitch(float y) {
    float percentage = y/grav;
    curr_state->velocity -= percentage * 0.1f;
    curr_state->pos->z +=  curr_state->velocity * 0.5f * percentage * TIME_UNIT;
}

/*
    Takes the current relative roll from the accelerometer and updates the position and heading of the plane
*/
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
