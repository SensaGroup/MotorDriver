#ifndef _STEPPER_H_
#define _STEPPER_H_

#include <avr/io.h>
#include <util/delay.h>
#include "system.h"

unsigned char init_stepper(void);
void enable_stepper(uint8_t state, uint8_t num);
void set_rpm_stepper(int rpm, uint8_t num);
void move_stepper(unsigned int steps, unsigned char dir, uint8_t num);


#endif
