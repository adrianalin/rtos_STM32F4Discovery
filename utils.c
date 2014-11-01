/*
 * utils.c
 *
 *  Created on: Oct 19, 2014
 *      Author: adrian
 */

#include "utils.h"

void initFeedbackLEDs()
{
#ifdef LEDS_DEBUG
	palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOD, 14);

	palSetPadMode(GPIOA, 9, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOA, 9);
#endif
}

void errorRecvMessageLED(void)
{
#ifdef LEDS_DEBUG
	// red LED
	palSetPad(GPIOD, 14);
	chThdSleepMilliseconds(10);
	palClearPad(GPIOD, 14);
#endif
}

void confirmMessageLED(void)
{
#ifdef LEDS_DEBUG
	// green LED
	palSetPad(GPIOA, 9);
	chThdSleepMilliseconds(10);
	palClearPad(GPIOA, 9);
#endif
}
