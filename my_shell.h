/*
 * shell.h
 *
 *  Created on: Jun 15, 2014
 *      Author: adrian
 */

#ifndef SHELL_H_
#define SHELL_H_

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "chprintf.h"
#include "shell.h"
#include "usbcfg.h"

#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

extern SerialUSBDriver SDU1;

void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]);
void my_shellInit(void);

#endif /* SHELL_H_ */
