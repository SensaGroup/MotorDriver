#include "system.h"

/*
 * Function:        void system_init(void)
 * Description:     init everything that is needed
 */
void system_init(void) {

    #ifdef DEBUG
        USART0_init(9600);
    #endif
    debug_printf("SENSA - Motor control board - DEBUG \n\n");
    debug_printf("Starting init... ");
    
    init_led();

    // indicate the beginning of the init
    blink_led(PIN_LED1);                        

    // CAN Bus init
    CAN_Init();
	if( CAN_InitRxMob(MOTOR_CONTROL_CAN_ID) == NOTINITED ) {
        debug_printf("ERROR! void system_init(void) - CAN Rx Mob not inited... halting system");
        while(1) {
            flip_led(PIN_LED1);
            flip_led(PIN_LED2);
            delay(100);
        }
    }

    // Stepper Init
    init_stepper();

    // DC Motor Init
    init_hbridge();

    // Encoder Init

    // Switches Init


    // end of init
    blink_led(PIN_LED2);

    debug_printf("finished... \n\n");
}

/************************************************************/
/*                   Helper functions                       */
/************************************************************/

/*
 * Function:   void delay(int ms)
 * Parameter:  int ms - the amount of miliseconds we need to delay
 */
void delay(int ms)
{
  while (0 < ms)
  {  
    _delay_ms(1);
    --ms;
  }
} // end of delay(int ms)

/************************************************************/
/*                  LED debug section                       */
/************************************************************/

/*
 * Function:   unsigned char init_led(void)
 * Parameter:  init the pins for the LEDs
 */
unsigned char init_led(void) {

    gpio_register_pin(PIN_LED1, GPIO_DIRECTION_OUTPUT, false);      // set led1 as output
    gpio_register_pin(PIN_LED2, GPIO_DIRECTION_OUTPUT, false);      // set led2 as output

    gpio_write_pin(PIN_LED1, LOW);                                  // set led1 low
    gpio_write_pin(PIN_LED2, LOW);                                  // set led2 low
    
    return 1;
    
} // end of init_led(void)

/*
 * Function:   void flip_led(uint8_t num)
 * Parameter:  change state of an led
 */
void flip_led(uint8_t num) {

    gpio_write_pin(num, !gpio_read_pin(num));           // writes the inverse of the current status 0->1, 1->0

} // end of flip_led(uint8_t num)

/*
 * Function:   void blink_led(uint8_t num) 
 * Parameter:  blink a certain LED
 */
void blink_led(uint8_t num) {

    flip_led(num);
    delay(100);
    flip_led(num);
    delay(100);

} // end of blink_led(uint8_t num)

/************************************************************/