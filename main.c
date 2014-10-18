#include "ch.h"
#include "hal.h"

// my includes
#include "communication.h"
#include "accelgiro.h"
#include "PWMsetup.h"

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

	initUSART2();

//	if(!init_i2c2())
//		return 1;

//	startPWM();

	while(1)
	{
		checkUSART2Messages();
//		getRawAccelGyro();

		chThdSleepMilliseconds(500);
	}
}
