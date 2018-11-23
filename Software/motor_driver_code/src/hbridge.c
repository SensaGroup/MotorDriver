#include "hbridge.h"

/*
 *  Function: unsigned int init_hbridge(void)
 *  Description: init the hbridge
 */
unsigned char init_hbridge(void) {

    // set everything to output
    gpio_register_pin(PIN_MOTOR1_A, GPIO_DIRECTION_OUTPUT, false);
    gpio_register_pin(PIN_MOTOR1_B, GPIO_DIRECTION_OUTPUT, false);
    gpio_register_pin(PIN_MOTOR2_A, GPIO_DIRECTION_OUTPUT, false);
    gpio_register_pin(PIN_MOTOR2_B, GPIO_DIRECTION_OUTPUT, false);			

    ICR3 = 5000;

	TCNT3 = 0;
	TCNT0 = 0;

	OCR3A = 0;
	OCR3B = 0;
	OCR3C = 0;

	TCCR3A = (1 << COM3A1)  | (1 << COM3B1) | (1 << COM3C1) | (1 << WGM31);
	TCCR3B = (1<< CS31) | (1 << WGM32) | (1 << WGM33); 		// PRESKALER = 8

    TCCR0A |= (1<<WGM00) | (1<<WGM01) | (1<<COM0A1) | (1<<CS00);

    hbridge_move(0,0,1);
    hbridge_move(0,0,2);

    return 1;
} // end of init_hbridge()

/*
 * Function: void hbridge_move(int A_value, int B_value, uint8_t num);
 * Description: move the hbridge by A_value and B_value. The ID is num
 */
void hbridge_move(int A_value, int B_value, uint8_t num) {
    
   debug_printf("INFO! hbridge_move - [A_value, B_value, num] - ");
   debug_printf_num(A_value);
   debug_printf(",");
   debug_printf_num(B_value);
   debug_printf(",");
   debug_printf_num(num);
   debug_printf("\n");

    switch(num) {
        case 1:
            OCR3A = (uint16_t)(conv_to_16(A_value));    // PE3
            OCR3B = (uint16_t)(conv_to_16(B_value));    // PE4

            if(A_value == 0) {
                gpio_write_pin(PIN_MOTOR1_A, LOW);
            } 
            
            if(B_value == 0) {
                gpio_write_pin(PIN_MOTOR1_B, LOW);
            }

            break;
        case 2:
            OCR3C = (uint16_t)(conv_to_16(A_value));    // PE5
            OCR0A = (uint8_t)(conv_to_8(B_value));      // PB7
            
            if(A_value == 0) {
                gpio_write_pin(PIN_MOTOR2_A, LOW);
            } 
            
            if(B_value == 0) {
                gpio_write_pin(PIN_MOTOR2_B, LOW);
            }

            break;
    }
} // end of hbridge_move()

/*
 * Function: void hbridge_timed(unsigned int ms, int achieve, uint8_t dir, uint8_t num)
 * Description: linear speed up/down at a certain period (ms)
 */
void hbridge_timed(unsigned int ms, int achieve, uint8_t dir, uint8_t num) {

    debug_printf("INFO! hbridge_timed - starting timed move...\n");

    int timing = 200; // ms the speed will be updated
    int current_A, current_B;
    int spd_A, spd_B = 0;

    // every timingms: (gain_A / ms(duration)) * timingms

    current_A = 0; current_B = 0;

    for(int i=0; ms/timing; i++) {

        // calculate the current speed
        if(num == 1) {
            current_A = conv_from_16(OCR3A);
            current_B = conv_from_16(OCR3B);
        } else if(num == 2){
            current_A = conv_from_16(OCR3C);
            current_B = conv_from_8(OCR0A);
        }            

        // spd = current + (((achieve - current) / time) * timing)
        // example: to increment speed, we set the achieve to a higher number.
        //          spd = 50 + (((80 - 50) / 2000) * 200)
        //          spd = 50 + 1,5 -> the speed will be incremented on every loop by 1,5
        // example: to decrease speed
        //          spd = 50 + (((20 - 50) / 2000) * 200)
        //          spd = 50 - 1,5 -> the speed will be decreased on every loop by 1

        if(dir == CW) {
            spd_A = current_A + (((achieve - current_A) / ms) * timing);
            spd_B = 0;
        } else {
            spd_A = 0;
            spd_B = current_B + (((achieve - current_B) / ms) * timing);
        }

        hbridge_move(spd_A, spd_B, num);

        delay(timing);
    }
} // end of hbridge_timed()