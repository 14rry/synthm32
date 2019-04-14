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

//#define LOOPBACK_TEST


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
    UART2_Init(); // used to print debug messages on serial port
    Timer_Init(); // used for rotary encoder, PWM for DAC, and periodic timer

    ST7735_Init();

    debug_printf(&HUART2, "Program started %d \n\r",0);

    ST7735_FillScreen(ST7735_BLACK);

    void loop() {
        // Check border
        ST7735_FillScreen(ST7735_BLACK);

        for(int x = 0; x < ST7735_WIDTH; x++) {
            ST7735_DrawPixel(x, 0, ST7735_RED);
            ST7735_DrawPixel(x, ST7735_HEIGHT-1, ST7735_RED);
        }

        for(int y = 0; y < ST7735_HEIGHT; y++) {
            ST7735_DrawPixel(0, y, ST7735_RED);
            ST7735_DrawPixel(ST7735_WIDTH-1, y, ST7735_RED);
        }

        HAL_Delay(3000);

        // Check fonts
        ST7735_FillScreen(ST7735_BLACK);
        ST7735_WriteString(0, 0, "Font_7x10, red on black, lorem ipsum dolor sit amet", Font_7x10, ST7735_RED, ST7735_BLACK);
        ST7735_WriteString(0, 3*10, "Font_11x18, green, lorem ipsum", Font_11x18, ST7735_GREEN, ST7735_BLACK);
        ST7735_WriteString(0, 3*10+3*18, "Font_16x26", Font_16x26, ST7735_BLUE, ST7735_BLACK);
        HAL_Delay(2000);

        // Check colors
        ST7735_FillScreen(ST7735_BLACK);
        ST7735_WriteString(0, 0, "BLACK", Font_11x18, ST7735_WHITE, ST7735_BLACK);
        HAL_Delay(500);

        ST7735_FillScreen(ST7735_BLUE);
        ST7735_WriteString(0, 0, "BLUE", Font_11x18, ST7735_BLACK, ST7735_BLUE);
        HAL_Delay(500);

        ST7735_FillScreen(ST7735_RED);
        ST7735_WriteString(0, 0, "RED", Font_11x18, ST7735_BLACK, ST7735_RED);
        HAL_Delay(500);

        ST7735_FillScreen(ST7735_GREEN);
        ST7735_WriteString(0, 0, "GREEN", Font_11x18, ST7735_BLACK, ST7735_GREEN);
        HAL_Delay(500);

        ST7735_FillScreen(ST7735_CYAN);
        ST7735_WriteString(0, 0, "CYAN", Font_11x18, ST7735_BLACK, ST7735_CYAN);
        HAL_Delay(500);

        ST7735_FillScreen(ST7735_MAGENTA);
        ST7735_WriteString(0, 0, "MAGENTA", Font_11x18, ST7735_BLACK, ST7735_MAGENTA);
        HAL_Delay(500);

        ST7735_FillScreen(ST7735_YELLOW);
        ST7735_WriteString(0, 0, "YELLOW", Font_11x18, ST7735_BLACK, ST7735_YELLOW);
        HAL_Delay(500);

        ST7735_FillScreen(ST7735_WHITE);
        ST7735_WriteString(0, 0, "WHITE", Font_11x18, ST7735_BLACK, ST7735_WHITE);
        HAL_Delay(500);
    }

    while (1)
    {
        loop();
    }

} // end main
