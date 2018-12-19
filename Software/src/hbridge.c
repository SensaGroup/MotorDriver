#include "hbridge.h"

int current_A_1, current_B_1, current_A_2, current_B_2;

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
            // convert to register
            OCR3C = (uint16_t)(conv_to_16(A_value));    // PE5
            OCR0A = (uint8_t)(conv_to_8(B_value));      // PB7
            
            // if one of it is 0, then just write a LOW
            if(A_value == 0) {
                gpio_write_pin(PIN_MOTOR2_A, LOW);
            } 
            
            if(B_value == 0) {
                gpio_write_pin(PIN_MOTOR2_B, LOW);
            }

            // update the local variable
            current_A_2 = A_value;
            current_B_2 = B_value;

            break;
        case 2:
            // convert to register
            OCR3A = (uint16_t)(conv_to_16(A_value));    // PE3
            OCR3B = (uint16_t)(conv_to_16(B_value));    // PE4

            // if one of it is 0, then just write a LOW
            if(A_value == 0) {
                gpio_write_pin(PIN_MOTOR1_A, LOW);
            } 
            
            if(B_value == 0) {
                gpio_write_pin(PIN_MOTOR1_B, LOW);
            }

            // update the local variable
            current_A_1 = A_value;
            current_B_1 = B_value;

            break;
    }
} // end of hbridge_move()

/*
 * Function:    void hbridge_break(uint8_t num)
 * Description: break the `num` hbridge
 */
void hbridge_break(uint8_t num) {
    hbridge_move(100, 100, num);
} // end of hbridge_break()

/*
 * Function:    void hbridge_break(uint8_t num)
 * Description: break the `num` hbridge
 */
void hbridge_time(int num, int dir, int achieve, int time) {
    
    // calculate the difference from the current and achieve speed
    int dif = 0;

    // switch between 1 and 2 motor
    switch(num) {
        case 1:
            // CW direction
            if(dir == CW) {

                // one side is always OFF
                current_A_1 = 0;                                        

                // calculate the difference
                dif = achieve - current_B_1;

                // for loop depends on how long we want to increment
                for(int i=0; i < time/100;i++) {
                    // current = current + (difference / (time / 100))
                    // we always increment the speed by a portion of the time (linear)
                    current_B_1 += dif/(time/100);
                    
                    // write to hbridge
                    hbridge_move(current_A_1, current_B_1, num);
                    delay(100);
                }
            } 
            // CCW direction
            else if(dir == CCW) {

                // one side is always OFF
                current_B_1 = 0;

                // calculate the difference
                dif = achieve - current_A_1;

                // for loop depends on how long we want to increment
                for(int i=0; i < time/100;i++) {
                    // current = current + (difference / (time / 100))
                    current_A_1 += dif/(time/100);

                    // write to hbridge
                    hbridge_move(current_A_1, current_B_1, num);
                    delay(100);
                }
            }
            break;
        case 2:
            // CW direction
            if(dir == CW) {

                // one side is always OFF
                current_A_2 = 0;

                // calculate the difference
                dif = achieve - current_B_2;

                // for loop depends on how long we want to increment
                for(int i=0; i < time/100;i++) {
                    // current = current + (difference / (time / 100))
                    current_B_2 += dif/(time/100);

                    // write to hbridge
                    hbridge_move(current_A_2, current_B_2, num);
                    delay(100);
                }
            }
            // CCW direction
            else if(dir == CCW) {

                // one side is always OFF
                current_B_2 = 0;

                // calculate the difference
                dif = achieve - current_A_2;

                // for loop depends on how long we want to increment
                for(int i=0; i < time/100;i++) {
                    // current = current + (difference / (time / 100))
                    current_A_2 += dif/(time/100);

                    // write to hbridge
                    hbridge_move(current_A_2, current_B_2, num);
                    delay(100);
                }
            }
            break;
    }
} // end of hbridge_time()