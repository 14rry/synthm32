/*
 * timer.c
 *
 *  Created on: Mar 5, 2019
 *      Author: larry
 */

#include "stm32f1xx_hal.h"
#include "debug_utils.h"
#include "uart.h"
#include "timer.h"
#include "sig_gen.h"

#define TIM2_TICK_FREQ (64000000) /*< Timer 4 tick frequency, configured in initialize.c */
#define TIM2_PERIOD (1279) /*< Period that gives a 50kHz timer rate. Works well with DAC */
#define TIM2_PULSE (319) /*< Initialize with 25% duty cycle */

// GLOBALS
volatile uint32_t EncoderCount = 0;
volatile uint16_t Autocount_Delay_ms = 1000;
volatile int DelayChanged = 0;
volatile int PeriodicTimerFlag = 0;

void Timer_Init()
{
    Initialize_Periodic_Timer();
    //Initialize_Encoder_Timer();
    Initialize_PWM_Timer();
}

void Initialize_Periodic_Timer()
{
    // initialize a 0.1 ms second timer
    TIM_HandleTypeDef *htimPtr = &htim_periodic;

    htimPtr->Instance = TIM4;
    htimPtr->Init.Prescaler = 63;
    htimPtr->Init.CounterMode = TIM_COUNTERMODE_UP;
    htimPtr->Init.Period = C1;

    //htimPtr->State = HAL_TIM_STATE_RESET;

    if (!HAL_TIM_Base_Init (htimPtr) == HAL_OK)
    {
        Error_Handler();
    }

    TIM_ClockConfigTypeDef clockConfig = {0};
    clockConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

    if (!HAL_TIM_ConfigClockSource(htimPtr,&clockConfig) == HAL_OK)
    {
        Error_Handler();
    }

}


void Initialize_PWM_Timer()
{
    /* based on https://www.simonmartin.ch/resources/stm32/dl/STM32%20Tutorial%2002%20-%20PWM%20Generation%20using%20HAL%20(and%20FreeRTOS).pdf */

    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;
    htim_pwm.Instance = TIM2;
    htim_pwm.Init.Prescaler = 0;
    htim_pwm.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim_pwm.Init.Period = TIM2_PERIOD;
    htim_pwm.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (!HAL_TIM_PWM_Init(&htim_pwm) == HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim_pwm, &sMasterConfig);
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = TIM2_PULSE;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim_pwm, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim_pwm, &sConfigOC, TIM_CHANNEL_2);

    if (!HAL_TIM_PWM_Start(&htim_pwm, TIM_CHANNEL_2) == HAL_OK)
    {
        Error_Handler();
    }
}

void Initialize_Encoder_Timer()
{
    // initialize timer 3 to read encoders

    TIM_HandleTypeDef *htimPtr = &htim_encoder;
    TIM_Encoder_InitTypeDef encoderConfig = {0};

    htimPtr->Instance = TIM3;
    htimPtr->Init.Prescaler = 0;
    htimPtr->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htimPtr->Init.CounterMode = TIM_COUNTERMODE_UP;
    htimPtr->Init.Period = 65535; // maximum possible value

    encoderConfig.EncoderMode = TIM_ENCODERMODE_TI1; // only use one channel
                                                    // less accurate but more
                                                    // consistent

    encoderConfig.IC1Filter = 10;
    encoderConfig.IC2Filter = 10;
    encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    //encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;

    if (HAL_TIM_Encoder_Init(htimPtr,&encoderConfig) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_Encoder_Start(htimPtr, TIM_CHANNEL_ALL);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == TIM4)
    {
        __HAL_RCC_TIM4_CLK_ENABLE();

        HAL_NVIC_SetPriority(TIM4_IRQn,0,0);
        HAL_NVIC_EnableIRQ(TIM4_IRQn);
    }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == TIM2) // PWM Timer
    {
        __HAL_RCC_TIM2_CLK_ENABLE();

        // enable GPIO pins for TIM2 channels 1 and 2 on PA0/PA1
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitTypeDef PWM_IO_init = {0};
        PWM_IO_init.Pin = GPIO_PIN_0|GPIO_PIN_1;
        PWM_IO_init.Mode = GPIO_MODE_AF_PP;
        PWM_IO_init.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOA, &PWM_IO_init);
    }
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        // timer 3 pins are PA6 and PA7
        GPIO_InitTypeDef timer3_IO_init = {0};
        timer3_IO_init.Pin = GPIO_PIN_6|GPIO_PIN_7;
        timer3_IO_init.Mode = GPIO_MODE_INPUT;
        timer3_IO_init.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &timer3_IO_init);

        //HAL_NVIC_SetPriority(TIM3_IRQn,0,0);
        //HAL_NVIC_EnableIRQ(TIM3_IRQn);
    }
}

volatile uint32_t EncoderCount;
void Check_Encoder()
{

    uint32_t currentCount = htim_encoder.Instance->CNT;
    if (EncoderCount != currentCount)
    {
        int diff = currentCount - EncoderCount;
        if (diff > 10000) // cw
        {
            Increase_Note();
        }
        else if (diff < -10000) // ccw
        {
            Decrease_Note();
        }
        else if (diff < 0) // cw
        {
            Increase_Note();
        }
        else // ccw
        {
            Decrease_Note();
        }

        EncoderCount = currentCount;
    }
}

