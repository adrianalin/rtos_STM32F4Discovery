#include "ch.h"
#include "hal.h"

#include <string.h>

// my includes
#include "communication.h"
#include "my_shell.h"
#include "PWMsetup.h"

// global vars
extern SerialUSBDriver SDU1;
const ShellCommand commands[] = {
		{"mem", cmd_mem},
		{"threads", cmd_threads},
		{"test", cmd_test},
		{NULL, NULL}
};
const ShellConfig shell_cfg = {
		(BaseSequentialStream *)&SDU1,
		commands
};

int main(void) {
	Thread *shelltp = NULL;

	uint8_t txbuf[2];
	uint8_t rxbuf[2];
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

	// init shell over USB (/dev/ttyACM0, 115200 bps)
	my_shellInit();

	/*
	 * Activates the serial driver 2 using the driver default configuration.
	 * PA2(TX) and PA3(RX) are routed to USART2.
	 */
	init_USART2();

	// initializes the PWM and starts it
	startPWM();

	while(1)
	{
		len=sdAsynchronousRead(&SD2,sbuf2,10);
		if(len>=1)
			sdWrite(&SD2, "sal", strlen("sal"));

		if (!shelltp) {
			if (SDU1.config->usbp->state == USB_ACTIVE) {
				/* Spawns a new shell.*/
				shelltp = shellCreate(&shell_cfg, SHELL_WA_SIZE, NORMALPRIO);
			}
		}
		else {
			/* If the previous shell exited.*/
			if (chThdTerminated(shelltp)) {
				/* Recovers memory of the previous shell.*/
				chThdRelease(shelltp);
				shelltp = NULL;
			}
		}
		chThdSleepMilliseconds(500);
	}
}
