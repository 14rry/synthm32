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

uint32_t NoteOnTime_mS = 0;
uint32_t NoteOffTime_mS = 0;
int IsNoteOn = 0;
float EnvelopeMod = 0; // percentage of full amplitude to output

// FUNCTION PROTOTYPES
int apply_ADSR(int);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    Update_Output_Signal();


}

void Initialize_Sig_Gen()
{
    Calculate_Buffers();
    Selected_Signal = Sine;

    SignalIndex = 0;
    ATTACK_mS = 100;
    RELEASE_mS = 500;
    AMPLITUDE_MOD = 80;

    // initialize the periodic timer for the first sine note
    __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, SineBuffer[SignalIndex]);

    HAL_TIM_Base_Start_IT(&htim_periodic); // wavetable timer
}

void Update_Output_Signal()
{
    // go to next signal sample
    SignalIndex++;
    if (SignalIndex >= NUM_SAMPLES)
    {
        SignalIndex = 0;
    }

    int amplitude = 0;

    if (Selected_Signal == Sine)
    {
        amplitude = SineBuffer[SignalIndex];
    }
    else if (Selected_Signal == Triangle)
    {
        amplitude = TriangleBuffer[SignalIndex];
    }
    else if (Selected_Signal == Square)
    {
        amplitude = SquareBuffer[SignalIndex];
    }

    amplitude = apply_ADSR(amplitude);

    __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, amplitude);


}

int apply_ADSR(int amp)
{
    float outputAmp = 0;
    if (IsNoteOn == 1)
    {
        uint32_t delta_time_mS = HAL_GetTick() - NoteOnTime_mS;

        if (delta_time_mS < ATTACK_mS)
        {
            // linear slope from 0 to MAX_AMP
            EnvelopeMod = (float)delta_time_mS/(float)ATTACK_mS;
            outputAmp = (float)amp*EnvelopeMod;
        }
        else
        {
            EnvelopeMod = 1;
            outputAmp = amp;
        }
    }
    else if (IsNoteOn == 0 && NoteOffTime_mS > 0) // note off
    {
        uint32_t delta_time_mS = HAL_GetTick() - NoteOffTime_mS;

        if (delta_time_mS < RELEASE_mS)
        {
            outputAmp = ((-((float)amp*EnvelopeMod)/(float)RELEASE_mS) * (float)delta_time_mS)
                    +((float)amp*EnvelopeMod);

        }
        else
        {
            outputAmp = 0; // release has elapsed, kill output
        }
    }

    return (int)(outputAmp*(float)AMPLITUDE_MOD/100.0f);
}

void Note_On(uint noteARR)
{
    // change timer frequency to match note_val
    __HAL_TIM_SET_AUTORELOAD(&htim_periodic,noteARR);

    // update time for ADSR envelope
    NoteOnTime_mS = HAL_GetTick();
    IsNoteOn = 1;

}

void Note_Off()
{
    NoteOffTime_mS = HAL_GetTick();
    IsNoteOn = 0;
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
