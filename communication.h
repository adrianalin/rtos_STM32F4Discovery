#ifndef SERIALCONFIG
#define SERIALCONFIG

#include "ch.h"
#include "hal.h"

// init USART; to be used with the JYMCU (serial bluetooth)
void initUSART2(void);
void printn(int16_t n);
void println(char *p);
void print(char *p);
void checkUSART2Messages(void);

#endif
