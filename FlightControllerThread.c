/*
 * flightcontroller.c
 *
 *  Created on: Nov 1, 2014
 *      Author: adrian
 */

#include "FlightControllerThread.h"
#include "PWMsetup.h"

extern int bluetoothCommand[3];

__attribute__((noreturn)) msg_t flightControllerThread(void *arg)
{
	startPWM();
	memset(bluetoothCommand, 0, sizeof(bluetoothCommand));

	while (1) {
		setMotor0DC(bluetoothCommand[0]);
		setMotor1DC(bluetoothCommand[1]);
		setMotor2DC(bluetoothCommand[2]);
		setMotor3DC(0);
		chThdSleepMilliseconds(50);
	}
}

