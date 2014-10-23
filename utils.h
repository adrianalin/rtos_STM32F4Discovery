/*
 * utils.h
 *
 *  Created on: Oct 18, 2014
 *      Author: adrian
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "hal.h"
#include "ch.h"

int my_atoi(char *p);

void initFeedbackLEDs(void);

// error receiving message - Red LED - on 500ms - OFF
void errorRecvMessage(void);

// Confirm the received message - Green LED - ON 500ms - OFF
void confirmMessage(void);

#endif /* UTILS_H_ */
