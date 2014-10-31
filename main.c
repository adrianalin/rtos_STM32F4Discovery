#include "ch.h"
#include "hal.h"

// my includes
#include "communication.h"
#include "accelgiro.h"
#include "PWMsetup.h"
#include "utils.h"

static WORKING_AREA(checkUSART_WA, 128);

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

//	if(!init_i2c2())
//		return 1;

//	startPWM();

	/* Launches checkUSART thread, with the working area checkUSART_WA and no arguments */
	chThdCreateStatic(checkUSART_WA, sizeof(checkUSART_WA),
			NORMALPRIO, threadCheckUSART2Messages, NULL);

	while(1)
	{
//		getRawAccelGyro();

		chThdSleepMilliseconds(500);
	}
}
