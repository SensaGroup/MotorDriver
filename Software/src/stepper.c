#include "stepper.h"

static int stepper_rpm[2] = {60, 60};

#define PIN_STEPPER1_DIR            16  // PC0
#define PIN_STEPPER1_STEP           17  // PC1
#define PIN_STEPPER1_EN             18  // PC2

#define PIN_STEPPER2_DIR            19  // PC3
#define PIN_STEPPER2_STEP           20  // PC4
#define PIN_STEPPER2_EN             21  // PC5

#define rtn_dir(num) (num == 1)? PIN_STEPPER1_DIR: PIN_STEPPER2_DIR
#define rtn_step(num) (num == 1)? PIN_STEPPER1_STEP: PIN_STEPPER2_STEP
#define rtn_en(num) (num == 1)? PIN_STEPPER1_EN: PIN_STEPPER2_EN

/*
 * Function:    unsigned char init_stepper()
 * Description: init the stepper pins
 */
unsigned char init_stepper(void) {
    
    gpio_register_pin(PIN_STEPPER1_DIR, GPIO_DIRECTION_OUTPUT, false);
    gpio_register_pin(PIN_STEPPER1_STEP, GPIO_DIRECTION_OUTPUT, false);
 //   gpio_register_pin(PIN_STEPPER1_EN, GPIO_DIRECTION_OUTPUT, false);

    gpio_register_pin(PIN_STEPPER2_DIR, GPIO_DIRECTION_OUTPUT, false);
    gpio_register_pin(PIN_STEPPER2_STEP, GPIO_DIRECTION_OUTPUT, false);
   // gpio_register_pin(PIN_STEPPER2_EN, GPIO_DIRECTION_OUTPUT, false);

    // enable drivers
   // enable_stepper(LOW, 1);
    //enable_stepper(LOW, 2);

    return 1;

} // end of init_stepper()

/*
 *  Function:       void enable_stepper(uint8_t state, uint8_t num)
 *  Description:    enable or disable the stepper driver
 */
void enable_stepper(uint8_t state, uint8_t num) {

    gpio_write_pin(rtn_en(num), state);

} // end of enable_stepper()

/*
 * Function:        void set_rpm_stepper(unsigned int rpm, uint8_t num)
 * Description:     set the rpm of the stepper
 */
void set_rpm_stepper(int rpm, uint8_t num) {

    stepper_rpm[num-1] = rpm;

} // end of set_rpm_stepper()

/*
 * Function:        void move_stepper(unsigned int steps, unsigned char dir, uint8_t num)
 * Description:     move the stepper for steps at a certain direction 
 */
void move_stepper(unsigned int steps, unsigned char dir, uint8_t num) {
    /*

    Delay between each step for the rpm and step:
    60000/(RPM*STEP) = x, where x is the time between each step

    */
    //int time_calculate = 60000 / (stepper_rpm[num-1] * steps);

    gpio_write_pin(rtn_dir(num), dir);

    for(unsigned int i=0; i<steps; i++) {
        gpio_write_pin(rtn_step(num), ON);
        _delay_ms(1);
        //delay(time_calculate/2);
        gpio_write_pin(rtn_step(num), OFF);
        _delay_ms(1);
        //delay(time_calculate/2);
    }
} // end of move_stepper()

// add encoder support here