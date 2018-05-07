#include "board_accelerometer.h"
#include "fsl_debug_console.h"
#include "board.h"

ACCELEROMETER_STATE state;
int main(){
	hardware_init();
	Accelerometer_Initialize();
	while(1){
		Accelerometer_GetState(&state);
		debug_printf("%5d %5d %5d\r\n", state.x, state.y, state.z);
		for (int i = 0; i < 500000; i++);
	}
	
}