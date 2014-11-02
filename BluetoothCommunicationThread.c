/*
 * serialConfig.c
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "BluetoothCommunicationThread.h"

int bluetoothCommand[3];
#define ROBOT_NAME "aerial"
#define BLUETOOTH_SPEED 115200
/*
  The posible baudrates are:
    AT+BAUD1-------1200
    AT+BAUD2-------2400
    AT+BAUD3-------4800
    AT+BAUD4-------9600 - Default for hc-06
    AT+BAUD5------19200
    AT+BAUD6------38400
    AT+BAUD7------57600
    AT+BAUD8-----115200
    AT+BAUD9-----230400
    AT+BAUDA-----460800
    AT+BAUDB-----921600
    AT+BAUDC----1382400
*/

void configureBluetoothDevice(void)
{
	char response[20];
	int readTimeout = 5000;

	memset(response, 0, sizeof(response));
	// Should respond with OK
	sdWrite(&SD2, "AT", strlen("AT"));
	sdReadTimeout(&SD2, response, sizeof(response), readTimeout);

	memset(response, 0, sizeof(response));
	// Should respond with its version
	sdWrite(&SD2, "AT+VERSION", strlen("AT+VERSION"));
	sdReadTimeout(&SD2, response, sizeof(response), readTimeout);

	memset(response, 0, sizeof(response));
	// Set the name to ROBOT_NAME
	sdWrite(&SD2, "AT+NAME", strlen("AT+NAME"));
	sdWrite(&SD2, ROBOT_NAME, strlen(ROBOT_NAME));
	sdReadTimeout(&SD2, response, sizeof(response), readTimeout);

	memset(response, 0, sizeof(response));
	// Set baudrate to 57600
	sdWrite(&SD2, "AT+BAUD8", strlen("AT+BAUD8"));
	sdReadTimeout(&SD2, response, sizeof(response), readTimeout);

	memset(response, 0, sizeof(response));
}

void print(char *p) {

	while (*p) {
		sdPut(&SD2, *p++);
	}
}

void println(char *p) {

	while (*p) {
		sdPut(&SD2, *p++);
	}
	sdWriteTimeout(&SD2, (uint8_t *)"\r\n", 2, TIME_INFINITE);
}

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
		BLUETOOTH_SPEED // bit rate
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
	// "111i0i0S"
	static char* auxMessage;
	static int i = 0, j = 0;
	static int command[3], commandIndex = 0;
	static char commandChar[5];
	i = 0;
	j = 0;
	commandIndex = 0;
	auxMessage = message;
	memset(command, 0, sizeof(command));
	memset(commandChar, 0, sizeof(commandChar));

	while ((auxMessage[i] != 'S') && (i < messageLength)) {
		if (auxMessage[i] == 'i') {
			command[commandIndex] = strtol(commandChar, NULL, 10);
			commandIndex++;
			j = 0;
			memset(commandChar, 0, sizeof(commandChar));
		} else {
			commandChar[j] = auxMessage[i];
			j++;
		}
		i++;
	}
	command[commandIndex] = strtol(commandChar, NULL, 10);

	if (commandIndex == 2) {
		memcpy(bluetoothCommand, command, sizeof(command));
		confirmMessageLED();
	}
}

__attribute__((noreturn)) msg_t bluetoothCommunicationThread(void *arg)
{
	char message[20];
	int recvLen;
	const int readTimeout = 50;

	initFeedbackLEDs();
	initUSART2();

	while (1)
	{
		memset(message, 0x00, sizeof(message));
		recvLen = sdReadTimeout(&SD2, (uint8_t*) message, sizeof(message), readTimeout);

		if (recvLen >= 5) {
			if (message[recvLen - 1] == 'S') {
				parseMessage(message, recvLen);
			}
		}

		chThdSleepMilliseconds(50);
	}
}
