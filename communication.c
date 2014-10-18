/*
 * serialConfig.c
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#include <stdlib.h>
#include <string.h>
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

static SerialConfig serialCfg =
{
		9600 // bit rate
};

void initUSART2(void)
{
	// ATTENTION! - power the JY-MCU (serial bluetooth) from 5V, GND (not VDD, GND)
	// PD5(TX) and PD6(RX) are routed to USART2
	palSetPadMode(GPIOD, 5, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(7));

	// Activates the serial driver 2 using the driver default configuration.
	sdStart(&SD2, &serialCfg);
}

void checkUSART2Messages()
{
	char buf[3];
	char message[] = "Ok\r\n";
	int bufLen=0, messageLen=0;

	memset(buf, 0x00, sizeof(buf));

	bufLen = chnRead(&SD2, (uint8_t*) buf, sizeof(buf));
	if (bufLen>=1)
		messageLen = chnWrite(&SD2, (uint8_t* ) message, strlen(message));
}
