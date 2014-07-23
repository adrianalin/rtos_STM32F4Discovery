/*
 * serialConfig.c
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#include <stdlib.h>
#include "communication.h"

/**
 *
 */
void print(char *p) {

	while (*p) {
		sdPut(&SD2, *p++);
	}
}

/**
 *
 */
void println(char *p) {

	while (*p) {
		sdPut(&SD2, *p++);
	}
	sdWriteTimeout(&SD2, (uint8_t *)"\r\n", 2, TIME_INFINITE);
}

/**
 *
 */
void printn(int16_t n) {
	char buf[16], *p;

	if (n > 0)
		sdPut(&SD2, '+');
	else{
		sdPut(&SD2, '-');
		n = abs(n);
	}

	if (!n)
		sdPut(&SD2, '0');
	else {
		p = buf;
		while (n)
			*p++ = (n % 10) + '0', n /= 10;
		while (p > buf)
			sdPut(&SD2, *--p);
	}
}

void init_USART2(void)
{
	SerialConfig sConfig2;

	sConfig2.speed = 9600;
	sConfig2.cr1 = 0;
	sConfig2.cr2 = USART_CR2_STOP1_BITS | USART_CR2_LINEN;
	sConfig2.cr3 = 0;

	//	PA2(TX) and PA3(RX) are routed to USART2
	palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

	//	Activates the serial driver 2 using the driver default configuration.
	sdStart(&SD2, &sConfig2);
}
