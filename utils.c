/*
 * utils.c
 *
 *  Created on: Oct 19, 2014
 *      Author: adrian
 */

#include "utils.h"

void initFeedbackLEDs()
{
	palSetPadMode(GPIOA, 9, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOA, 9);
}

void confirmMessageLED(void)
{
	// green LED
	palSetPad(GPIOA, 9);
	chThdSleepMilliseconds(10);
	palClearPad(GPIOA, 9);
}
