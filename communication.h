#ifndef SERIALCONFIG
#define SERIALCONFIG

#include "ch.h"
#include "hal.h"

void printn(int16_t n);
void println(char *p);
void print(char *p);

// USART messages thread
__attribute__((noreturn)) msg_t threadCheckUSART2Messages(void *arg);

#endif
