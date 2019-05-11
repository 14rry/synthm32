/*
 * midi.c
 *
 *  Created on: May 11, 2019
 *      Author: larry
 */
#include "midi.h"
#include "stm32f1xx_hal.h"
#include "uart.h"
#include "debug_utils.h"

void MIDI_Init()
{
    MIDI_Rx_Buffer = 0;
    HAL_UART_Receive_IT(&HUART3,&MIDI_Rx_Buffer,1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    debug_printf(&HUART2, "Byte received: %d \n\r",MIDI_Rx_Buffer);

    // restart listen
    HAL_UART_Receive_IT(&HUART3,&MIDI_Rx_Buffer,1);
}
