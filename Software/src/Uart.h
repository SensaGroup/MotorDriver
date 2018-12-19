#ifndef USART0_H_
#define USART0_H_

#include <stdio.h>

void USART0_init(uint32_t baud);

void USART0_transmit(unsigned char data);
unsigned char USART0_receive(void);
void USART0_string(char *s);
void debug_printf(char *s);
void debug_printf_num(int num);

#endif
