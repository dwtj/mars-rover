#ifndef UTIL_H
#define UTIL_H

// Blocks for a specified number of milliseconds
void wait_ms(unsigned int time_val);

// Shaft encoder initialization
void shaft_encoder_init(void);

// Shaft encoder read function
signed char read_shaft_encoder(void);

// Initialize Stepper Motor
void stepper_init(void);

// Stepper motor move function
void  move_stepper_motor_by_step(int num_steps, int direction);

// Initialize PORTC, which is used by the push buttons
void init_push_buttons(void);


/**
 * Return the position of button being pushed.
 * @return the position of the left-most button being pushed.  A value of 1 indicates the rightmost button.  0 indicates no button being pressed
 */
unsigned char read_push_buttons(void);

unsigned char wait_button(char *mesg);

#endif /* UTIL_H */