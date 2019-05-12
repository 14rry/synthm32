/**
* @file main.c
* @author Larry Hixenbaugh
* @date 02/17/2019
* @brief This file implements HAL, IRQ, and debouncing for lab 3
*
* This file configures the system clock, configures GPIO pins to control
*  internal and external push buttons and LEDs. Push buttons trigger external
*  interrupts to manipulate a binary value displayed on the three external
*  LEDs. UARTs are used to accept external commands to control the counter.
*/
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "stm32f1xx.h"
#include "stm32f1xx_nucleo.h"
#include "stm32f1xx_hal.h"

#include "initialize.h"
#include "uart.h"
#include "debug_utils.h"
#include "timer.h"
#include "lcd_driver.h"

#include "st7735.h"
#include "fonts.h"
#include "testimg.h"

#include "GUI.h"
#include "sig_gen.h"

#include "MIDI.h"

/**
 * @brief Initializes board clock, GPIO, UART 2, and signal generator.
 * Calls functions to output analog signal and read rotary encoder.
 */
int main(void)
{
    HAL_Init();
    System_Clock_Init();
    SPI_Init();
    GPIO_Init(); // enable push buttons

    UART2_Init(); // used to print debug message on serial port
    UART3_Init(); // used to communicate with FTDI breakout board
    MIDI_Init();

    Timer_Init(); // used for rotary encoder, PWM for DAC, and periodic timer
    Initialize_Sig_Gen();

    ST7735_Init();

    debug_printf(&HUART2, "Program started %d \n\r",0);

    ST7735_FillScreen(ST7735_BLACK);
    Display_Main_Menu();

    uint32_t lastEncoderCount = htim_encoder.Instance->CNT;

    while (1)
    {
        Check_GUI_Buttons(HAL_GetTick());

        //GPIO_PinState test = HAL_GPIO_ReadPin(GPIOB, 2);
        //debug_printf(&HUART2,"Pin value: %d\n\r",test);

        if (lastEncoderCount != htim_encoder.Instance->CNT)
        {
            lastEncoderCount = htim_encoder.Instance->CNT;
            debug_printf(&HUART2,"Encoder value: %d\n\r",lastEncoderCount);
        }
    }

} // end main
