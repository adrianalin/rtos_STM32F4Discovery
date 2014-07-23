#include "ch.h"
#include "hal.h"

// my includes
#include "communication.h"
#include "accelgiro.h"
#include "PWMsetup.h"

int main(void) {
	uint8_t sbuf2[100];
	int len=0;

	/*
	 * System initializations.
	 * - HAL initialization, this also initializes the configured device drivers
	 *   and performs the board-specific initializations.
	 * - Kernel initialization, the main() function becomes a thread and the
	 *   RTOS is active.
	 */
	halInit();
	chSysInit();

	init_USART2();

	if(!init_i2c2())
		return 1;

	startPWM();

	while(1)
	{
		len=sdAsynchronousRead(&SD2,sbuf2,10);
		if(len>=1)
			sdWrite(&SD2, "sal", strlen("sal"));

		getRawAccelGyro();

		chThdSleepMilliseconds(500);
	}
}
