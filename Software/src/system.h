#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#ifndef F_CPU
    #define F_CPU 16000000UL
#endif

#include <avr/interrupt.h>
#include <util/atomic.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>

#include "gpio.h"
#include "system.h"
#include "Uart.h"
#include "hbridge.h"
#include "can.h"
#include "stepper.h"
#include "gpio.h"

/*
 * 0 PA0 		8  PB0 		16 PC0 		24 PD0 		32 PE0 		40 PF0		48 PG0
 * 1 PA1 		9  PB1 		17 PC1 		25 PD1 		33 PE1 		41 PF1		49 PG1
 * 2 PA2 		10 PB2 		18 PC2 		26 PD2 		34 PE2 		42 PF2      50 PG2
 * 3 PA3 		11 PB3 		19 PC3 		27 PD3 		35 PE3 		43 PF3      51 PG3
 * 4 PA4 		12 PB4 		20 PC4 		28 PD4 		36 PE4 		44 PF4
 * 5 PA5 		13 PB5 		21 PC5 		29 PD5 		37 PE5 		45 PF5
 * 6 PA6 		14 PB6 		22 PC6 		30 PD6 		38 PE6 		46 PF6
 * 7 PA7 		15 PB7 		23 PC7 		31 PD7 		39 PE7 		47 PF7
 */

#define DEBUG

#define ON          1
#define OFF         0
#define HIGH        1
#define LOW         0
#define OUTPUT      1
#define INPUT       0
#define CW          1
#define CCW         0
#define INITED      1
#define NOTINITED   0

void system_init(void);

void delay(int ms);
uint8_t read_switch(uint8_t num);

#define PIN_LED1                    48 // PG0
#define PIN_LED2                    49 // PG1


#endif
