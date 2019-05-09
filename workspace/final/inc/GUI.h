/*
 * GUI.h
 *
 *  Created on: Apr 13, 2019
 *      Author: larry
 */

#ifndef GUI_H_
#define GUI_H_

#include <stdio.h>

#define BUTTON_DELAY_mS (50) /** Button debounce delay in milliseconds */


uint32_t ButtonUpTime; /**< time of interrupt hit */
uint32_t ButtonDownTime; /**< time of interrupt hit */
uint32_t ButtonLeftTime; /**< time of interrupt hit */
uint32_t ButtonRightTime; /**< time of interrupt hit */

void Display_Main_Menu();
void Display_Menu(char * items, int num);
void Display_Menu_Cursor(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void Check_GUI_Buttons(uint32_t currentTime);
void Display_Waveform();


#endif /* GUI_H_ */
