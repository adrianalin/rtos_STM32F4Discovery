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

bool_t init_i2c2(void);
void DMPdata(void);
void getTemperature(void);

#endif /* ACCELGIRO_H_ */
