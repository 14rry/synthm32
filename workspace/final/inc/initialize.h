/**
* @file initialize.h
* @author Larry Hixenbaugh
* @date Feb 23, 2019
* @brief Functions to initialize system clock and GPIO buttons/LEDs
*
* Enables interrupts on the pushbutton input pins.
*/

#ifndef INITIALIZE_H_
#define INITIALIZE_H_

/**
 * @brief Initialize clock settings as defined in the lab assignment.
 *
 * clock/osc details on p.92 of RM0008
 * HSI is 8 MHz
 * 8 MHz gets divided by 2 and input into the PLL
 * PLL at 4 MHz, multiply by 16 to get 64 MHz clock
 * Divide by 2 to get APB1 32 MHz
 * According to lab assignment, we want:
 *  System Clock    64 MHz
 *  AHB             64 MHz
 *  APB1            32 MHz
 *  APB2            64 MHz
 */
void System_Clock_Init(void);

/**
 * @brief Initialize GPIO input pins for push-buttons and output pins for LEDs
 *
 * External momentary push-buttons are wired to be active low. In their
 * non-active state the buttons are floating (no dedicated pull-up resistor).
 * Input pins are thus configured as GPIO_PULLUP. External interrupts are
 * configured for all input push buttons.
 *
 * LEDs are configured as output pins
 */
void GPIO_Init(void);

#endif /* INITIALIZE_H_ */
