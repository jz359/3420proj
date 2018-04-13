#include <Board_Accelerometer.h>
#include <Board_Magnetometer.h>
#include <fsl_debug_console.h>
#include <board.h>

ACCELEROMETER_STATE state;
MAGNETOMETER_STATE mstate;

int main() {
	hardware_init();
	Accelerometer_Initialize();
	Magnetometer_Initialize();

	while(1) {
		Accelerometer_GetState(&state);
		Magnetometer_GetState(&mstate);
		debug_printf("%5d %5d %5d %5d %5d %5d\r\n", state.x, state.y, state.z);
	}
}