#ifndef SERIALCONFIG
#define SERIALCONFIG

#include "ch.h"
#include "hal.h"

// init USART; to be used with the JYMCU (serial bluetooth)
void init_USART2(void);
void printn(int16_t n);
void println(char *p);
void print(char *p);

#endif
