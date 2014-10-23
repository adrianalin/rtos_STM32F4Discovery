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
	palSetPadMode(GPIOD, 5, PAL_MODE_OUTPUT_PUSHPULL);
	palSetPad(GPIOD, 5);

	palSetPadMode(GPIOA, 9, PAL_MODE_OUTPUT_PUSHPULL);
	palClearPad(GPIOA, 9);
}

void errorRecvMessage(void)
{
	// red LED
	palClearPad(GPIOD, 5);
	chThdSleepMilliseconds(500);
	palSetPad(GPIOD, 5);
}

void confirmMessage(void)
{
	// green LED
	palSetPad(GPIOA, 9);
	chThdSleepMilliseconds(100);
	palClearPad(GPIOA, 9);
}
