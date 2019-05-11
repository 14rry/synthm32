/*
 * midi.c
 *
 *  Created on: May 11, 2019
 *      Author: larry
 */

#include "stm32f1xx_hal.h"
#include "uart.h"
#include "debug_utils.h"
#include "sig_gen.h"
#include "midi.h"



void MIDI_Init()
{
    MIDI_Rx_Buffer = 0;
    HAL_UART_Receive_IT(&HUART3,&MIDI_Rx_Buffer,1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //debug_printf(&HUART2, "Byte received: %d \n\r",MIDI_Rx_Buffer);

    // note on or note off?
    uint8_t message_type = (MIDI_Rx_Buffer & 0xF0) >> 4;
    if (message_type == MIDI_NOTE_OFF) // note off
    {
        Note_Off();
    }
    else if (message_type == MIDI_NOTE_ON)
    {
        HAL_UART_Receive(&HUART3,&MIDI_Rx_Buffer,1,0xFFFF); // read note val
        int note_val = MIDI_Rx_Buffer;

        if (!(note_val > MIDI_MAX_NOTE || note_val < MIDI_MIN_NOTE))
        {
            int noteARR = MIDI_ARR_LOOKUP[note_val - MIDI_MIN_NOTE];
            //int noteARR = 0;
            Note_On(noteARR);
        }

    }


    // restart listen
    HAL_UART_Receive_IT(&HUART3,&MIDI_Rx_Buffer,1);
}
