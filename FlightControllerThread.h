/*
 * flightcontroller.h
 *
 *  Created on: Nov 1, 2014
 *      Author: adrian
 */

#ifndef FLIGHTCONTROLLERTHREAD_H_
#define FLIGHTCONTROLLERTHREAD_H_

#include "ch.h"
#include "hal.h"

__attribute__((noreturn)) msg_t flightControllerThread(void *arg);

#endif /* FLIGHTCONTROLLERTHREAD_H_ */
