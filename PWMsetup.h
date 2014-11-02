/*
 * PWMsetup.h
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#ifndef PWMSETUP_H_
#define PWMSETUP_H_

#include "ch.h"
#include "hal.h"

void startPWM(void);
void setMotor0DC(int value);
void setMotor1DC(int value);
void setMotor2DC(int value);
void setMotor3DC(int value);

#endif /* PWMSETUP_H_ */
