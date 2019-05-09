/*
 * sig_gen.c
 *
 *  Created on: Mar 31, 2019
 *      Author: larry
 */
#include <math.h>
#include "sig_gen.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "timer.h"
#include "debug_utils.h"
#include "uart.h"

int SineBuffer[NUM_SAMPLES];
int TriangleBuffer[NUM_SAMPLES];
int SquareBuffer[NUM_SAMPLES];

int CurrentNoteIndex = 3; // starts on c1
int MajorScale[15] = {G0,A0,B0,C1,D1,E1,F1,G1,A1,B1,C2,D2,E2,F2,G2};

void Initialize_Sig_Gen()
{
    Calculate_Buffers();
    Selected_Signal = Sine;

    // light up LED1 to show that sine is selected
//    HAL_GPIO_WritePin(ULED1_PORT, ULED1, 1);
//    HAL_GPIO_WritePin(ULED2_PORT, ULED2, 0);
//    HAL_GPIO_WritePin(ULED3_PORT, ULED3, 0);

    SignalIndex = 0;

    // initialize the periodic timer for the first sine note
    __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, SineBuffer[SignalIndex]);
}

void Update_Output_Signal()
{
    if (PeriodicTimerFlag)
    {
        // go to next signal sample
        SignalIndex++;
        if (SignalIndex >= NUM_SAMPLES)
        {
            SignalIndex = 0;
        }

        if (Selected_Signal == Sine)
        {
            __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, SineBuffer[SignalIndex]);
        }
        else if (Selected_Signal == Triangle)
        {
            __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, TriangleBuffer[SignalIndex]);
        }
        else if (Selected_Signal == Square)
        {
            __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, SquareBuffer[SignalIndex]);
        }

        PeriodicTimerFlag = 0; // reset flag
    }

}

void Increase_Note()
{
    // don't go beyond number of notes in scale
    if (CurrentNoteIndex < SCALE_LENGTH-1)
    {
        uint32_t newARR = MajorScale[++CurrentNoteIndex];
        __HAL_TIM_SET_AUTORELOAD(&htim_periodic,newARR);
        debug_printf(&HUART2, "Increase note to: %d \n\r",newARR);

    }
}

void Decrease_Note()
{

    // don't go below zero
    if (CurrentNoteIndex > 0)
    {
        uint32_t newARR = MajorScale[--CurrentNoteIndex];
        __HAL_TIM_SET_AUTORELOAD(&htim_periodic,newARR);
        debug_printf(&HUART2, "Decrease note to: %d \n\r",newARR);
    }
}

void Calculate_Buffers()
{
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        SineBuffer[i] = AMPLITUDE*sin(2.0f*3.14159f*i/NUM_SAMPLES) + AMPLITUDE;

        if (i < NUM_SAMPLES/2)
        {
            TriangleBuffer[i] = 4*AMPLITUDE*i/NUM_SAMPLES;
            SquareBuffer[i] = 2*AMPLITUDE;
        }
        else
        {
            TriangleBuffer[i] = -4*AMPLITUDE*i/NUM_SAMPLES + AMPLITUDE*4;
            SquareBuffer[i] = 0;
        }
    }
}

void Change_Selected_Signal()
{
    if (Selected_Signal == Sine)
    {
        Selected_Signal = Triangle;
        HAL_GPIO_WritePin(ULED1_PORT, ULED1, 0);
        HAL_GPIO_WritePin(ULED2_PORT, ULED2, 1);
        HAL_GPIO_WritePin(ULED3_PORT, ULED3, 0);
    }
    else if (Selected_Signal == Triangle)
    {
        Selected_Signal = Square;
        HAL_GPIO_WritePin(ULED1_PORT, ULED1, 0);
        HAL_GPIO_WritePin(ULED2_PORT, ULED2, 0);
        HAL_GPIO_WritePin(ULED3_PORT, ULED3, 1);
    }
    else
    {
        Selected_Signal = Sine;
        HAL_GPIO_WritePin(ULED1_PORT, ULED1, 1);
        HAL_GPIO_WritePin(ULED2_PORT, ULED2, 0);
        HAL_GPIO_WritePin(ULED3_PORT, ULED3, 0);
    }

}
