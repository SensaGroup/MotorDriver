#ifndef _HBRIDGE_H_
#define _HBRIDGE_H_

#include <avr/io.h>
#include <util/delay.h>
#include "system.h"

#define PIN_MOTOR1_A 35 // PE3
#define PIN_MOTOR1_B 36 // PE4
#define PIN_MOTOR2_A 37 // PE5
#define PIN_MOTOR2_B 15 // PB7

#define conv_to_16(x) ((65535*x) / 100)
#define conv_from_16(x) ((x/65535)*100)

#define conv_to_8(x)  ((255*x) / 100)
#define conv_from_8(x) ((x/255) * 100)

unsigned char init_hbridge(void);
void hbridge_move(int A_value, int B_value, uint8_t num);
void hbridge_break(uint8_t num);
void hbridge_time(int num, int dir, int achieve, int time);

#endif