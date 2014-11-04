/*
 * accelgiro.h
 *
 *  Created on: Jul 23, 2014
 *      Author: adrianp
 */

#ifndef ACCELGIRO_H_
#define ACCELGIRO_H_

#include "ch.h"
#include "hal.h"

void initI2C2(void);
void initMPU(void);
void initDMP(void);
void getYPR(void);
void getTemperature(void);
void DMPdata(void);

#endif /* ACCELGIRO_H_ */
