/*
 * serialConfig.c
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "communication.h"

// global variables

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

static void initUSART2(void)
{
	// ATTENTION! - power the JY-MCU (serial bluetooth) from 5V, GND (not VDD, GND)
	// PD5(TX) and PD6(RX) are routed to USART2
	palSetPadMode(GPIOD, 5, PAL_MODE_ALTERNATE(7));
	palSetPadMode(GPIOD, 6, PAL_MODE_ALTERNATE(7));

	// Activates the serial driver 2 using the driver default configuration.
	sdStart(&SD2, &serialCfg);
}

static void parseMessage(char* message, int messageLength)
{
	// message =  i10i230i221S
	static const int len=3;
	static char nrChar;
	static int commands[3], cnt = 0, nr = 0;
	static char commandChar[3];
	static char ok = 0;

	memset(commands, 0, sizeof(commands));
	memset(commandChar, 0, sizeof(commandChar));
	cnt = 0;

	while (*message != 'S')
	{
		ok = 0;
		if (*message == 'i')
		{
			message++;
			// message =  10i230i221S
			nrChar = *message;
			nr = strtol(&nrChar, NULL, 10);
			message++;
			// message =  0i230i221S
			if (nr <= len)
			{
				memcpy(commandChar, message, nr);
				message = message + nr;
				commands[cnt] = strtol(commandChar, NULL, 10);
				memset(commandChar, 0, len);
				cnt++;
				ok = 1;
			}
		}
		if (!ok)
			break;
	}

	if (ok)
		confirmMessage();
}

__attribute__((noreturn)) msg_t threadCheckUSART2Messages(void *arg)
{
	char messageLengthChar[2], message[20];
	const int len = sizeof(messageLengthChar) / sizeof(char);
	const int maxMessageLength = sizeof(message) / sizeof(char);
	int recvLen, messageLength;
	const int readTimeout = 50;
	unsigned char ok = 0;

	initUSART2();

	while (1)
	{
		memset(message, 0x00, sizeof(message));

		recvLen = sdReadTimeout(&SD2, (uint8_t*) &messageLengthChar, len, readTimeout);
		if (recvLen == len)
		{
			ok = 0;
			messageLength = strtol(messageLengthChar, NULL, 10);
			if (messageLength <= maxMessageLength)
			{
				recvLen = sdReadTimeout(&SD2, (uint8_t*) &message, messageLength, readTimeout);
				if (message[messageLength - 1] == 'S')
				{
					ok = 1;
					parseMessage(message, messageLength);
				}
			}
		}
		chThdSleepMilliseconds(200);
		if (!ok)
			errorRecvMessage();
	}
}
