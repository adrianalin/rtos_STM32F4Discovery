#include "ch.h"
#include "hal.h"

// my includes
#include "BluetoothCommunicationThread.h"
#include "FlightControllerThread.h"
#include "accelgiro.h"

static WORKING_AREA(bluetoothCommunicationThread_WA, 128);
static WORKING_AREA(flightControllerThread_WA, 128);

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

	chThdCreateStatic(flightControllerThread_WA, sizeof(flightControllerThread_WA),
			NORMALPRIO, flightControllerThread, NULL);

	if(!init_i2c2())
		return 1;

	while(1)
	{
		DMPdata();
		chThdSleepMilliseconds(50);
	}
}
