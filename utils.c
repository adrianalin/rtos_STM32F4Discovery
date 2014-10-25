/*
 * utils.c
 *
 *  Created on: Oct 19, 2014
 *      Author: adrian
 */

#include "utils.h"

int my_atoi(char *p) {
	int k = 0;
	while (*p) {
		k = k*10 + (*p) - '0';
		p++;
	}
	return k;
}

void initFeedbackLEDs()
{
	palSetPadMode(GPIOD, 14, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOD, 14);

	palSetPadMode(GPIOA, 9, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOA, 9);
}

void errorRecvMessage(void)
{
	// red LED
	palSetPad(GPIOD, 14);
	chThdSleepMilliseconds(10);
	palClearPad(GPIOD, 14);
}

void confirmMessage(void)
{
	// green LED
	palSetPad(GPIOA, 9);
	chThdSleepMilliseconds(10);
	palClearPad(GPIOA, 9);
}
