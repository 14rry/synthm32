/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif
#include "stm32f1xx_it.h"
#include "uart.h"
#include "debug_utils.h"
#include "timer.h"
#include "initialize.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
#ifdef USE_RTOS_SYSTICK
	osSystickHandler();
#endif
}

void EXTI15_10_IRQHandler(void)
{
    if (HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_RIGHT_PIN) == 0)
    {
        HAL_GPIO_EXTI_IRQHandler(BUTTON_RIGHT_PIN);
    }
    else if (HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_UP_PIN) == 0)
    {
        HAL_GPIO_EXTI_IRQHandler(BUTTON_UP_PIN);
    }
    else if (HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_DOWN_PIN) == 0)
    {
        HAL_GPIO_EXTI_IRQHandler(BUTTON_DOWN_PIN);
    }
    else
    {
        // clear all irqs
        __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_RIGHT_PIN);
        __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_UP_PIN);
        __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_DOWN_PIN);
    }

}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void) // rotary encoder
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2); // only use one channel
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
}

//void EXTI4_IRQHandler(void)
//{
//    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4); // down
//}
//
//void EXTI1_IRQHandler(void)
//{
//    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1); // up
//}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&HUART1);
}

void USART3_IRQHandler(void)
{
    HAL_UART_IRQHandler(&HUART3);
}

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim_periodic);
}

