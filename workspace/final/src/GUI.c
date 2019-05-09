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
#include "sig_gen.h"
#include "graphics_ext.h"

#define MENU_PAD (8)
#define MENU_HEIGHT (56)
#define MENU_BG ST7735_YELLOW
#define MENU_FG ST7735_MAGENTA

int Menu_Selection = 1;
int Min_Sel = 1;
int Max_Sel = 3;

uint32_t ButtonUpTime = 0; /**< time of interrupt hit */
uint32_t ButtonDownTime = 0; /**< time of interrupt hit */
uint32_t ButtonLeftTime = 0; /**< time of interrupt hit */
uint32_t ButtonRightTime = 0;

void Clear_Menu_Area();
void Backward_Menu();

enum Menus { main, synth, oscope };
enum Menus CurrentMenu = main;

void Display_Main_Menu()
{
    Clear_Menu_Area();

    // Menu title
    ST7735_WriteString(0,0,"MAIN MENU:",Font_7x10, MENU_FG,
            MENU_BG);

    // Menu selections
    ST7735_WriteString(MENU_PAD, 10, "Synth Settings", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 20, "Graphics Demo", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 30, "Custom Functions", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 40, "Toggle Backlight", Font_7x10, MENU_FG,
                MENU_BG);


    Min_Sel = 1;
    Max_Sel = 4;
    Menu_Selection = 1;
    Display_Menu_Cursor();

    CurrentMenu = main;

    Display_Waveform_GUI();
}

void Display_Synth_Menu()
{
    Clear_Menu_Area();

    // Menu title:
    ST7735_WriteString(0,0,"SYNTH SETTINGS:",Font_7x10, MENU_FG,
            MENU_BG);

    // Menu selections
    ST7735_WriteString(MENU_PAD, 10, "Waveform", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 20, "Amplitude", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 30, "Release", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 40, "Return to MAIN", Font_7x10, MENU_FG,
            MENU_BG);

    Min_Sel = 1;
    Max_Sel = 4;
    Menu_Selection = 1;
    Display_Menu_Cursor();

    CurrentMenu = synth;
}

void Display_Waveform_GUI()
{

    Display_Waveform(0,
            MENU_HEIGHT+1,
            ST7735_WIDTH-1,
            ST7735_HEIGHT-1);

}

 void Display_Menu_Cursor()
 {
     ST7735_WriteString(0, Menu_Selection*10, ">", Font_7x10, MENU_FG,
             MENU_BG);
 }

 void Clear_Menu_Cursor()
 {
     ST7735_WriteString(0, Menu_Selection*10, " ", Font_7x10, MENU_FG,
             MENU_BG);
 }

 void Clear_Menu_Area()
 {
     ST7735_FillRectangle(0, 0, 160, MENU_HEIGHT, MENU_BG);
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

 void Forward_Menu()
 {
     switch(CurrentMenu)
      {
          case main:
              if (Menu_Selection == 1)
              {
                  Display_Synth_Menu();
              }
              else if (Menu_Selection == 2)
              {
                  graphics_test();
                  Display_Main_Menu();
              }
              else if (Menu_Selection == 3)
              {
                  Graphics_Demo();
                  Display_Main_Menu();
              }
              else if (Menu_Selection == 4)
              {
                  Toggle_LCD_Brightness();
              }
              break;
          case synth:
              if (Menu_Selection == 1)
              {
                  Change_Selected_Signal();
                  ST7735_FillRectangle(0,MENU_HEIGHT+2,64,
                          (ST7735_HEIGHT-MENU_HEIGHT-2)/2,ST7735_BLACK);

                  Display_Waveform_GUI();
              }
              else if (Menu_Selection == 4)
              {
                  Backward_Menu();
              }
              break;
          case oscope:
              break;
          default:
              break;
      }
 }

 void Backward_Menu()
 {
     switch(CurrentMenu)
     {
         case main:
             break;
         case synth:
             Display_Main_Menu();
             break;
         case oscope:
             Display_Main_Menu();
             break;
         default:
             Display_Main_Menu();
             break;
     }
 }


 // GPIO FUNCTIONS

 void Check_GUI_Buttons(uint32_t currentTime)
 {
     if(ButtonUpTime > 0)
     {
         if(currentTime > ButtonUpTime + BUTTON_DELAY_mS)
         {
             if (HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_UP_PIN) == 0)
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
             if (HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_DOWN_PIN) == 0)
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
             if (HAL_GPIO_ReadPin(BUTTON_PORT,BUTTON_RIGHT_PIN) == 0)
             {
                 Forward_Menu();
             }
             ButtonRightTime = 0;
         }
     }

     if(ButtonLeftTime > 0)
     {
         if(currentTime > ButtonLeftTime + BUTTON_DELAY_mS)
         {
             if (HAL_GPIO_ReadPin(BUTTON_LEFT_PORT,BUTTON_LEFT_PIN) == 0)
             {
                 Backward_Menu();
             }
             ButtonLeftTime = 0;
         }
     }
 }

 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 {
     if(GPIO_Pin == BUTTON_UP_PIN)
     {
         ButtonUpTime = HAL_GetTick();
     }
     if(GPIO_Pin == BUTTON_DOWN_PIN)
     {
         ButtonDownTime = HAL_GetTick();
     }
     if (GPIO_Pin == BUTTON_RIGHT_PIN)
     {
         ButtonRightTime = HAL_GetTick();
     }
     if (GPIO_Pin == BUTTON_LEFT_PIN)
     {
         ButtonLeftTime = HAL_GetTick();
     }
 }
