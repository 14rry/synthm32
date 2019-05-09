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

// internal function prototypes
void Clear_Menu_Area();
void Display_Unfilled_Rectangle(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);

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
    ST7735_WriteString(MENU_PAD, 20, "Oscilloscope", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 30, "Sequencer", Font_7x10, MENU_FG,
            MENU_BG);

    Min_Sel = 1;
    Max_Sel = 3;
    Menu_Selection = 1;
    Display_Menu_Cursor();

    CurrentMenu = main;
}

void Display_Synth_Menu()
{
    Clear_Menu_Area();

    // Menu title:
    ST7735_WriteString(0,0,"SYNTH SETTINGS:",Font_7x10, MENU_FG,
            MENU_BG);

    // Menu selections
    ST7735_WriteString(MENU_PAD, 10, "Waveform: Sine", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 20, "Amplitude: 10", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 30, "Release: 10", Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 40, "Return to MAIN", Font_7x10, MENU_FG,
            MENU_BG);

    Min_Sel = 1;
    Max_Sel = 4;
    Menu_Selection = 1;
    Display_Menu_Cursor();

    CurrentMenu = synth;
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
              break;
          case synth:
              if (Menu_Selection == 1)
              {
                  Change_Selected_Signal();
                  ST7735_FillRectangle(0,MENU_HEIGHT+2,64,
                          (ST7735_HEIGHT-MENU_HEIGHT-2)/2,ST7735_BLACK);
                  Display_Waveform();
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

 void Display_Waveform()
 {
     // draw outline
     Display_Unfilled_Rectangle(0,MENU_HEIGHT+1,ST7735_WIDTH-1,
             ST7735_HEIGHT-1,ST7735_WHITE);

     // draw actual waveform from buffer data

     // interpolate buffer to fit number of display points
     //int skip_size = NUM_SAMPLES/(ST7735_WIDTH-2);
     int repeat_amount = (ST7735_WIDTH-2)/NUM_SAMPLES;
     repeat_amount = 1;
     int display_x = 0;
     int wave_height = (ST7735_HEIGHT-MENU_HEIGHT-3)/2;
     int prev_y = -1;
     for (int j = 0; j < 2; j++)
     {
         for (int i = 0; i < NUM_SAMPLES; i++)
         {
             int display_y;
             switch(Selected_Signal)
             {
                 case Sine:
                     display_y = (SineBuffer[i]/(AMPLITUDE*2.0f)) * wave_height
                         + MENU_HEIGHT + 2;
                     break;
                 case Triangle:
                     display_y = (TriangleBuffer[i]/(AMPLITUDE*2.0f)) * wave_height
                         + MENU_HEIGHT + 2;
                     break;
                 case Square:
                     display_y = (SquareBuffer[i]/(AMPLITUDE*2.0f)) * wave_height
                         + MENU_HEIGHT + 2;
                     break;
                 default:
                     display_y = 0;
                     break;
             }

             if (prev_y > 0)
             {
                 if (display_y > prev_y)
                 {
                     ST7735_FillRectangle(display_x,prev_y,1,display_y-prev_y,ST7735_CYAN);
                 }
                 else if (display_y == prev_y)
                 {
                     ST7735_DrawPixel(display_x,display_y,ST7735_CYAN);
                 }
                 else
                 {
                     ST7735_FillRectangle(display_x,display_y,1,prev_y-display_y,ST7735_CYAN);
                 }
             }
             else
             {
                 ST7735_DrawPixel(display_x,display_y,ST7735_CYAN);
             }

             //ST7735_FillRectangle(display_x,display_y,repeat_amount,1,ST7735_CYAN);
             display_x += repeat_amount;
             prev_y = display_y;
         }
     }


 }

 void Display_Unfilled_Rectangle(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2,uint16_t color)
 {
     // horizontal
     ST7735_FillRectangle(x,y,x2,1,color);
     ST7735_FillRectangle(x,y2,x2,1,color);

     // vertical
     ST7735_FillRectangle(x,y,1,y2,color);
     ST7735_FillRectangle(x2,y,1,y2,color);
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
