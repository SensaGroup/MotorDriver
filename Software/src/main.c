#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "can.h"

canMsg message;
canMsg snd_message;

unsigned char dummy_data[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
unsigned char first_byte, second_byte;

static void can_send_done(void) {
	
	unsigned char ready_data[8];

	ready_data[0] = 1;
			 
	memcpy(snd_message.data, ready_data, 8);
	snd_message.msgIde = MAIN_BOARD_CAN_ID;

	if(CAN_write(snd_message) == 1) {
		// success
	}
} 

int main(void) {

	system_init();

	while(1) {

		if(CAN_checkRX() == 1) {
			CAN_read(&message);

			first_byte 		= message.data[0];
			second_byte 	= message.data[1];

			if(first_byte == 'P') {
					
				// SUB-ACTION: LED flash {'P', 0}
				if(second_byte == 0) {
							
					gpio_write_pin(PIN_LED2, true);
					delay(500);
					gpio_write_pin(PIN_LED2, false);
							
				} // end of SUB-ACTION: LED flash

				// SUB-ACTION: WITH REPLY {'P', 1}
				else if(second_byte == 1) {

					gpio_write_pin(PIN_LED2, true);
					delay(500);

					memcpy(snd_message.data, dummy_data, 8);
					snd_message.msgIde = MAIN_BOARD_CAN_ID;

					if(CAN_write(snd_message) == 1) {
						// success
						gpio_write_pin(PIN_LED2, false);
					}

				} // end of SUB-ACTION: WITH REPLY
			} // end of PING

			// ACTION: switches {'S'}
			else if(first_byte == 'S') {
				
				unsigned char switch_data[8];

				switch_data[0] = read_switch((uint8_t)(second_byte));
				
				memcpy(snd_message.data, switch_data, 8);
				snd_message.msgIde = MAIN_BOARD_CAN_ID;

				if(CAN_write(snd_message) == 1) {
					// success
				}

			} // end of switches

			// ACTION: h-bridge
			else if(first_byte == 'H') {

				// SUB-ACTION: move {'H', 'M', a >> 8, a & 0xFF, b >> 8, b & 0xFF, num}
				if(second_byte == 'M') {

					int _a, _b; unsigned char num;

					_a 	= (message.data[2] << 8) | message.data[3];
					_b 	= (message.data[4] << 8) | message.data[5];
					num = message.data[6];

					hbridge_move(_a, _b, (uint8_t)(num));

					can_send_done();

				} // end of move

				// SUB-ACTION: break {'H','B', num}
				else if(second_byte == 'B') {
					
					hbridge_break((uint8_t)(message.data[2]));

					can_send_done();

				} // end of break

				// SUB-ACTION: time {'H', 'T/t', num, dir >> 8, dir & 0xFF, achieve >> 8, achieve & 0xFF, time >> 8, time & 0xFF,}
				else if(second_byte == 'T' || second_byte == 't') {

					unsigned char _num; int _dir, _achieve, _time;
					
					if(second_byte == 'T') {
						_num = 0;
					} else {
						_num = 1;
					}
					
					_dir 		= (message.data[2] << 8) | message.data[3];
					_achieve 	= (message.data[4] << 8) | message.data[5];
					_time 		= (message.data[6] << 8) | message.data[7];

					hbridge_time((int)(_num), _dir, _achieve, _time);

					can_send_done();

				} // end of time
			} // end of h-bridge

			// ACTION: stepper motor
			else if(first_byte == 's') {
				
				// SUB-ACTION: enable {'s', 'E', state, num}
				if(second_byte == 'E') {
					
					uint8_t state, num;

					state 	= (uint8_t)(message.data[2]);
					num 	= (uint8_t)(message.data[3]);

					enable_stepper(state, num);

					can_send_done();

				} // end of enable

				// SUB-ACTION: rpm {'s', 'R', rpm >> 8, rpm & 0xFF, num}
				else if(second_byte == 'R') {

					int _rpm; uint8_t num;

					_rpm = (message.data[2] << 8) | message.data[3];
					num = (uint8_t)(message.data[4]);

					set_rpm_stepper(_rpm, num);

					can_send_done();

				} // end of rpm

				// SUB-ACTION: move {'s', 'M', steps >> 8, rpm & 0xFF, dir, num}
				else if(second_byte == 'M') {

					int steps; unsigned char dir; uint8_t num;

					steps = (message.data[2] << 8) | message.data[3];
					dir = message.data[4];
					num = (uint8_t)(message.data[5]);

					move_stepper(steps, dir, num);

					can_send_done();

				} // end of move
			} // end of stepper motor
		} // end of checkRX
	} // end of while

  	return 0;

}