/*
 * midi.h
 *
 *  Created on: May 11, 2019
 *      Author: larry
 */

#ifndef MIDI_H_
#define MIDI_H_

uint8_t MIDI_Rx_Buffer;

#define MIDI_NOTE_OFF (0b1000)
#define MIDI_NOTE_ON (0b1001)

#define MIDI_MIN_NOTE (48)
#define MIDI_MAX_NOTE (83)

// MIDI NOTE VALUES AND CORRESPONDING TIMER ARR VALUES
static const int MIDI_ARR_LOOKUP[36] = {
    238,
    224,
    212,
    200,
    189,
    178,
    168,
    158,
    149,
    141,
    133,
    126,
    118,
    112,
    105,
    99,
    94,
    88,
    83,
    79,
    74,
    70,
    66,
    62,
    59,
    55,
    52,
    49,
    46,
    44,
    41,
    39,
    37,
    35,
    33,
    31
};

void MIDI_Init();
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);


#endif /* MIDI_H_ */
