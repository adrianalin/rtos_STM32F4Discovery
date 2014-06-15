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

void startPWM(void) {
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
}
