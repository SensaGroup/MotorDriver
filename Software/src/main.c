#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "system.h"

int main(void) {

	system_init();
	 
	canMsg message;

	while(1) {
		CAN_read(&message);
		if(message.data[0] == 'A') {
			if(message.data[1] == 0) {
				gpio_write_pin(PIN_LED2, false);
			} else {
				gpio_write_pin(PIN_LED2, true);
			}
			
		}
	}

  	return 0;

}