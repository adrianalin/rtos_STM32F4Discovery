#ifndef SERIALCONFIG
#define SERIALCONFIG

#include "ch.h"
#include "hal.h"

void init_i2c2(void);

// init USART; to be used with the JYMCU (serial bluetooth)
void init_USART2(void);

#endif
