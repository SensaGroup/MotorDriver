#include "system.h"

static volatile unsigned long sys_time;
static void (*timer_callback)(void) = NULL;

static void init_switches(void);
static void init_led(void);
static void timer_init(unsigned int freq);

/*
 * Function:        void timer_register_callback(void (*callback)(void))
 * Description:     assign function to timer callback
 */
void timer_register_callback(void (*callback)(void))
{
    timer_callback = callback;
} // end of timer_register_callback()

/*
 * Function:        static void timer_init(unsigned int freq)
 * Description:     setup the timer by `freq`
 */
static void timer_init(unsigned int freq)
{
    TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS10);
	OCR1A = (double)F_CPU / (double)freq + 0.5;
	TIMSK1 = 1 << OCIE1A;

	SREG |= 0x80;
} // end of timer_init()

ISR(TIMER1_COMPA_vect)
{
    // if timer_callback function is set, execute it
    if(timer_callback != NULL)
        timer_callback();

    // increment sys_time
    sys_time++;

}

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
    gpio_write_pin(PIN_LED1, HIGH);                       

    CAN_Init();
	CAN_InitRxMob(MOTOR_CONTROL_CAN_ID);

    // timer callback for debouncing inputs
    timer_register_callback(gpio_debouncer);

    // timer set to 1000hz
    timer_init(1000);


    // Stepper Init
    init_stepper();

    // DC Motor Init
    init_hbridge();

    // Encoder Init

    // Switches Init
    init_switches();

    // end of init
    gpio_write_pin(PIN_LED1, LOW); 

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
 * Function:   void init_led(void)
 * Parameter:  init the pins for the LEDs
 */
static void init_led(void) {

    gpio_register_pin(PIN_LED1, GPIO_DIRECTION_OUTPUT, true);      // set led1 as output
    gpio_register_pin(PIN_LED2, GPIO_DIRECTION_OUTPUT, true);      // set led2 as output

    gpio_write_pin(PIN_LED1, LOW);                                  // set led1 low
    gpio_write_pin(PIN_LED2, LOW);                                  // set led2 low
    
} // end of init_led(void)


/************************************************************/
/*                   Switches section                       */
/************************************************************/

uint8_t switches_pin[8] = {40,41,42,43,44,45,46,47};

/*
 * Function:   static void init_switches()
 * Parameter:  init pins for switches (switches_pin)
 */
static void init_switches(void) {
    for(int i=0; i < 8; i++) {
        gpio_register_pin(switches_pin[i], GPIO_DIRECTION_INPUT, false);
    }
} // end of init_switches()

/*
 * Function:   uint8_t read_switch(uint8_t num)
 * Parameter:  read `num` switch
 */
uint8_t read_switch(uint8_t num) {

    return gpio_read_pin(switches_pin[num] - 1);

} // end of read_switch()

/************************************************************/