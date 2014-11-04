#include "ch.h"
#include "hal.h"
#include "PWMsetup.h"

// my includes
#include "BluetoothCommunicationThread.h"
#include "accelgiro.h"

static WORKING_AREA(bluetoothCommunicationThread_WA, 128);

int main(void) {
	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	chThdCreateStatic(bluetoothCommunicationThread_WA, sizeof(bluetoothCommunicationThread_WA),
			NORMALPRIO, bluetoothCommunicationThread, NULL);

	initI2C2();
	initMPU();
	initDMP();

	startPWM();

	while(1)
	{
		getYPR();
		chThdSleepMilliseconds(50);
	}
}
