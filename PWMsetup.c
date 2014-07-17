/*
 * PWMsetup.c
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#include "PWMsetup.h"

/*
 * PWM cyclic callback.
 */
static void pwmpcb(PWMDriver *pwmp) {
	(void)pwmp;
}

/*
 * PWM configuration structure.
 * the active state is a logic one.
 */
static PWMConfig pwmcfg = {
		1000000,
		20000, 		// 50 Hz PWM frequency
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

void startPWM(void) {
	/*
	 * Initializes the PWM driver 4
	 */
	pwmStart(&PWMD4, &pwmcfg);

	/*
	 * routes the TIM4 outputs to the board LEDs.
	 */
//	palSetPadMode(GPIOD, GPIOD_LED3, PAL_MODE_ALTERNATE(2));  /* Orange.  */
//	palSetPadMode(GPIOD, GPIOD_LED4, PAL_MODE_ALTERNATE(2));  /* Green.   */
//	palSetPadMode(GPIOD, GPIOD_LED5, PAL_MODE_ALTERNATE(2));  /* Red.     */
//	palSetPadMode(GPIOD, GPIOD_LED6, PAL_MODE_ALTERNATE(2));  /* Blue.    */
//
//	pwmEnableChannelI(&PWMD4, 0, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 200));
//	pwmEnableChannelI(&PWMD4, 1, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 300));
//	pwmEnableChannelI(&PWMD4, 2, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 400));
//	pwmEnableChannelI(&PWMD4, 3, PWM_FRACTION_TO_WIDTH(&PWMD4, 4096, 500));

	/*
	 * routes the TIM4 outputs to PB6, PB7, PB8, PB9 (Motors)
	 */
	palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOB, 8, PAL_MODE_ALTERNATE(2));
	palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(2));

	/* Start up PWMs so ESCs can initialize.
	 * You usually need to start it out at 1mS (ie motor off) for several seconds to arm the ESC.
	 */
	pwmEnableChannel(&PWMD4, 0, duty);
	pwmEnableChannel(&PWMD4, 1, duty);
	pwmEnableChannel(&PWMD4, 2, duty);
	pwmEnableChannel(&PWMD4, 3, duty);

	// wait 3 sechonds to initialize ESC
	chThdSleepMilliseconds(5000);

	// set duty cycle between 1ms (1000) and 2200 (2ms)
	pwmEnableChannel(&PWMD4, 0, 2300);
	pwmEnableChannel(&PWMD4, 1, 2300);
	pwmEnableChannel(&PWMD4, 2, 2300);
	pwmEnableChannel(&PWMD4, 3, 2300);
}
