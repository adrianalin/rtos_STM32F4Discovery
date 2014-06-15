#include "ch.h"
#include "hal.h"
#include <string.h>
#include <stdlib.h>
#include "test.h"

#include "chprintf.h"
#include "shell.h"
#include "lis302dl.h"

#include "usbcfg.h"

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;

/*===========================================================================*/
/* Command line related.                                                     */
/*===========================================================================*/

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
	size_t n, size;

	(void)argv;
	if (argc > 0) {
		chprintf(chp, "Usage: mem\r\n");
		return;
	}
	n = chHeapStatus(NULL, &size);
	chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
	chprintf(chp, "heap fragments   : %u\r\n", n);
	chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
	static const char *states[] = {THD_STATE_NAMES};
	Thread *tp;

	(void)argv;
	if (argc > 0) {
		chprintf(chp, "Usage: threads\r\n");
		return;
	}
	chprintf(chp, "    addr    stack prio refs     state time\r\n");
	tp = chRegFirstThread();
	do {
		chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
				(uint32_t)tp, (uint32_t)tp->p_ctx.r13,
				(uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
				states[tp->p_state], (uint32_t)tp->p_time);
		tp = chRegNextThread(tp);
	} while (tp != NULL);
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]) {
	Thread *tp;

	(void)argv;
	if (argc > 0) {
		chprintf(chp, "Usage: test\r\n");
		return;
	}
	tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(),
			TestThread, chp);
	if (tp == NULL) {
		chprintf(chp, "out of memory\r\n");
		return;
	}
	chThdWait(tp);
}

static const ShellCommand commands[] = {
		{"mem", cmd_mem},
		{"threads", cmd_threads},
		{"test", cmd_test},
		{NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
		(BaseSequentialStream *)&SDU1,
		commands
};

/*
 * PWM cyclic callback.
 */
static void pwmpcb(PWMDriver *pwmp) {
	(void)pwmp;
}

/*
 * PWM configuration structure.
 * Cyclic callback enabled, channels 1 and 4 enabled without callbacks,
 * the active state is a logic one.
 */
static PWMConfig pwmcfg = {
		10000,                                    /* 10kHz PWM clock frequency.   */
		10000,                                    /* PWM period 1S (in ticks).    */
		pwmpcb,
		{
				{PWM_OUTPUT_ACTIVE_HIGH, NULL},
				{PWM_OUTPUT_ACTIVE_HIGH, NULL},
				{PWM_OUTPUT_ACTIVE_HIGH, NULL},
				{PWM_OUTPUT_ACTIVE_HIGH, NULL}
		},
		/* HW dependent part.*/
		0,
		0
};

void init_USART2(void){
	SerialConfig sConfig2;

	sConfig2.speed = 9600;
	sConfig2.cr1 = 0;
	sConfig2.cr2 = USART_CR2_STOP1_BITS | USART_CR2_LINEN;
	sConfig2.cr3 = 0;

	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	sdStart(&SD2, &sConfig2);
}

//static const I2CConfig i2ccfg = {
//		OPMODE_I2C,
//		400000,
//		FAST_DUTY_CYCLE_2,
//};

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

	/*
	 * Shell manager initialization.
	 */
	shellInit();

	/*
	 * Initializes a serial-over-USB CDC driver.
	 */
	sduObjectInit(&SDU1);
	sduStart(&SDU1, &serusbcfg);

	/*
	 * Activates the USB driver and then the USB bus pull-up on D+.
	 * Note, a delay is inserted in order to not have to disconnect the cable
	 * after a reset.
	 */
	usbDisconnectBus(serusbcfg.usbp);
	chThdSleepMilliseconds(1000);
	usbStart(serusbcfg.usbp, &usbcfg);
	usbConnectBus(serusbcfg.usbp);

	/*
	 * Activates the serial driver 2 using the driver default configuration.
	 * PA2(TX) and PA3(RX) are routed to USART2.
	 */
	init_USART2();

	/*
	 * Initializes the PWM driver 4, routes the TIM4 outputs to the board LEDs.
	 */
	pwmStart(&PWMD4, &pwmcfg);
	palSetPadMode(GPIOD, GPIOD_LED3, PAL_MODE_ALTERNATE(2));  /* Orange.  */
	palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_ALTERNATE(2));  /* Green.   */
	palSetPadMode(GPIOD, GPIOD_LED5, PAL_MODE_ALTERNATE(2));  /* Red.     */
	palSetPadMode(GPIOD, GPIOD_LED6, PAL_MODE_ALTERNATE(2));  /* Blue.    */

	pwmEnableChannelI(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 200));
	pwmEnableChannelI(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 300));
	pwmEnableChannelI(&PWMD4, 2, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 400));
	pwmEnableChannelI(&PWMD4, 3, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 500));

	//	//I2C setup
	//	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(4)  | PAL_STM32_OTYPE_OPENDRAIN);   /* SCL */
	//	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(4)  | PAL_STM32_OTYPE_OPENDRAIN);   /* SDA */
	//	txbuf[0] = 0x00;
	//	txbuf[0] = 0x00;
	//	i2cStart(&I2CD1, &i2ccfg);
	//	i2cAcquireBus(&I2CD1);
	//	i2cMasterTransmit(&I2CD1, 0x82 >> 1, txbuf, 1, rxbuf, 2);
	//	i2cReleaseBus(&I2CD1);
	//	chThdSleepMilliseconds(500);

	while(1)
	{
		//		len=sdAsynchronousRead(&SD2,sbuf2,10);
		//		if(len>=1)
		//			sdWrite(&SD2, "sal", strlen("sal"));
//		if (!shelltp) {
//			if (SDU1.config->usbp->state == USB_ACTIVE) {
//				/* Spawns a new shell.*/
//				shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
//			}
//		}
//		else {
//			/* If the previous shell exited.*/
//			if (chThdTerminated(shelltp)) {
//				/* Recovers memory of the previous shell.*/
//				chThdRelease(shelltp);
//				shelltp = NULL;
//			}
//		}
//		chThdSleepMilliseconds(500);
	}

}
