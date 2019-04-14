/*
 * GUI.c
 *
 *  Created on: Apr 13, 2019
 *      Author: larry
 */
#include "GUI.h"
#include "stm32f1xx_hal.h"
#include "st7735.h"
#include <string.h>
#include "initialize.h"

#define MENU_PAD (8)

int Menu_Selection = 1;
int Min_Sel = 1;
int Max_Sel = 3;

uint32_t ButtonUpTime = 0; /**< time of interrupt hit */
uint32_t ButtonDownTime = 0; /**< time of interrupt hit */
uint32_t ButtonLeftTime = 0; /**< time of interrupt hit */
uint32_t ButtonRightTime = 0;

void Display_Main_Menu()
{
    // Menu title
    ST7735_WriteString(0,0,"MAIN MENU:",Font_7x10, ST7735_WHITE,
            ST7735_BLACK);

    // Menu selections
    ST7735_WriteString(MENU_PAD, 10, "Synth Settings", Font_7x10, ST7735_WHITE,
            ST7735_BLACK);
    ST7735_WriteString(MENU_PAD, 20, "Oscilloscope", Font_7x10, ST7735_WHITE,
            ST7735_BLACK);
    ST7735_WriteString(MENU_PAD, 30, "Sequencer", Font_7x10, ST7735_WHITE,
            ST7735_BLACK);

    Min_Sel = 1;
    Max_Sel = 3;
    Menu_Selection = 1;
    Display_Menu_Cursor();
}
 void Display_Menu_Cursor()
 {
     ST7735_WriteString(0, Menu_Selection*10, ">", Font_7x10, ST7735_WHITE,
             ST7735_BLACK);
 }

 void Clear_Menu_Cursor()
 {
     ST7735_WriteString(0, Menu_Selection*10, " ", Font_7x10, ST7735_WHITE,
             ST7735_BLACK);
 }

 void Increase_Selection()
 {
     if (Menu_Selection < Max_Sel)
     {
         Clear_Menu_Cursor();
         Menu_Selection++;
         Display_Menu_Cursor();
     }

 }

 void Decrease_Selection()
 {
     if (Menu_Selection > Min_Sel)
     {
         Clear_Menu_Cursor();
         Menu_Selection--;
         Display_Menu_Cursor();
     }
 }

 void Check_GUI_Buttons(uint32_t currentTime)
 {
     if(ButtonUpTime > 0)
     {
         if(currentTime > ButtonUpTime + BUTTON_DELAY_mS)
         {
             if (((HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_UP_PIN)-1)*-1) == 1)
             {
                 Decrease_Selection();
             }
             ButtonUpTime = 0; // reset count whether or not input was accepted
         }
     }

     if(ButtonDownTime > 0)
     {
         if(currentTime > ButtonDownTime + BUTTON_DELAY_mS)
         {
             if (((HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_DOWN_PIN)-1)*-1) == 1)
             {
                 Increase_Selection();
             }
             ButtonDownTime = 0;
         }
     }

     if(ButtonRightTime > 0)
     {
         if(currentTime > ButtonRightTime + BUTTON_DELAY_mS)
         {
             if (((HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_RIGHT_PIN)-1)*-1) == 1)
             {
                 //Decrement_Count();
             }
             ButtonRightTime = 0;
         }
     }

     if(ButtonLeftTime > 0)
     {
         if(currentTime > ButtonLeftTime + BUTTON_DELAY_mS)
         {
             if (((HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_LEFT_PIN)-1)*-1) == 1)
             {
                 //Decrement_Count();
             }
             ButtonLeftTime = 0;
         }
     }
 }



 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 {
     if(GPIO_Pin == BUTTON_UP_PIN && Menu_Selection > Min_Sel)
     {
         ButtonUpTime = HAL_GetTick();
     }
     if(GPIO_Pin == BUTTON_DOWN_PIN && Menu_Selection < Max_Sel)
     {
         ButtonDownTime = HAL_GetTick();
     }
 }
