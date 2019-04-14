/**
* @file initialize.c
* @author Larry Hixenbaugh
* @date Feb 23, 2019
* @brief Functions to initialize system clock and GPIO buttons/LEDs
*
* Enables interrupts on the pushbutton input pins.
*/

#include "stm32f1xx.h"
#include "debug_utils.h"
#include "initialize.h"


void System_Clock_Init(void)
{
    /* create required structs for oscillator and clock initialization
     * from page 56 of HAL description document*/
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClockInitStruct;

    // populate the structs with the desired settings
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClockInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2; // config all at once
    RCC_ClockInitStruct.SYSCLKSource = RCC_CFGR_SW_PLL;
    RCC_ClockInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClockInitStruct.APB1CLKDivider = RCC_HCLK_DIV2; // divide by 2 for 32MHz
    RCC_ClockInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    /* according to RM0008, a sysclock of 64 MHz requires latency of 2 wait
     * cycles
     */
    if (HAL_RCC_ClockConfig(&RCC_ClockInitStruct,FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void configure_buttons(void)
{
    /**
    * From example on page 58 of HAL documentation and module 3 lecture nodes
    */
    GPIO_InitTypeDef GPIO_InitStruct;

    // Configure blue button (onboard)
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // active low
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure external push-buttons
    GPIO_InitStruct.Pin = BUTTON_UP_PIN | BUTTON_DOWN_PIN | BUTTON_LEFT_PIN |
            BUTTON_RIGHT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // active low
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_PORT, &GPIO_InitStruct);
}

void configure_LEDs(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Configure LED2 (onboard)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//    // Configure External LEDs
//    GPIO_InitStruct.Pin = ULED1;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(ULED1_PORT, &GPIO_InitStruct);
//
//    GPIO_InitStruct.Pin = ULED2;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(ULED2_PORT, &GPIO_InitStruct);
//
//    GPIO_InitStruct.Pin = ULED3;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(ULED3_PORT, &GPIO_InitStruct);
}

void GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    //__HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    configure_buttons();
    //configure_LEDs();

    // Set up interrupts
    HAL_NVIC_SetPriority(EXTI15_10_IRQn,0,0); // blue button and left
    HAL_NVIC_SetPriority(EXTI3_IRQn,2,0); // right
    HAL_NVIC_SetPriority(EXTI4_IRQn,1,0); // down
    HAL_NVIC_SetPriority(EXTI9_5_IRQn,3,0); // up

    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

