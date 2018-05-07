#include <fsl_device_registers.h>
#include "constants.h"

// stationary accelerations
int x_0 = 0;
int y_0 = -40;
int z_0 = 990;

unsigned int threshold = 40;
unsigned int board_size = 100;
unsigned int time_interval = DEFAULT_SYSTEM_CLOCK / 100;
unsigned int time_int_ms = 10;