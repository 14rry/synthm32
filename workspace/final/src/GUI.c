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
#include "lcd_driver.h"
#include "debug_utils.h"
#include "uart.h"

#define MENU_PAD (8)
#define MENU_HEIGHT (66)
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
void Update_Menu_Item(int selection,char* str);
void Reverse_Encoder_Menu();
void Adjust_Parameter(int direction);

enum Menus { main, synth, oscope };
enum Menus CurrentMenu = main;

int EncoderState[2] = {0,0};
int EncoderVal = 0;

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
    char disp_string[80];
    sprintf(disp_string,"Attack: %d",ATTACK_mS);
    ST7735_WriteString(MENU_PAD, 20, disp_string, Font_7x10, MENU_FG,
            MENU_BG);

    sprintf(disp_string,"Release: %d",RELEASE_mS);
    ST7735_WriteString(MENU_PAD, 30, disp_string, Font_7x10, MENU_FG,
            MENU_BG);
    sprintf(disp_string,"Amplitude: %d",AMPLITUDE_MOD);
    ST7735_WriteString(MENU_PAD, 40, disp_string, Font_7x10, MENU_FG,
            MENU_BG);
    ST7735_WriteString(MENU_PAD, 50, "Return to MAIN", Font_7x10, MENU_FG,
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
              else if (Menu_Selection == 5)
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

 void Adjust_Parameter(int direction)
 {
     if (CurrentMenu != synth)
     {
         return; // only adjust synth settings with encoder
     }

     char disp_string[80];
     if (Menu_Selection == 2) // attack
     {
         ATTACK_mS += direction*ADJUST_AMOUNT;

         if (ATTACK_mS < MIN_PARAMETER_TIME)
         {
             ATTACK_mS = MIN_PARAMETER_TIME;
         }
         else if (ATTACK_mS > MAX_PARAMETER_TIME)
         {
             ATTACK_mS = MAX_PARAMETER_TIME;
         }

         sprintf(disp_string,"Attack: %d",ATTACK_mS);
     }
     else if (Menu_Selection == 3) // release
     {
         RELEASE_mS += direction*ADJUST_AMOUNT;

         if (RELEASE_mS < MIN_PARAMETER_TIME)
         {
             RELEASE_mS = MIN_PARAMETER_TIME;
         }
         else if (RELEASE_mS > MAX_PARAMETER_TIME)
         {
             RELEASE_mS = MAX_PARAMETER_TIME;
         }

         sprintf(disp_string,"Release: %d",RELEASE_mS);
     }
     else if (Menu_Selection == 4)
     {
         AMPLITUDE_MOD += direction*10;

          if (AMPLITUDE_MOD < 1)
          {
              AMPLITUDE_MOD = 0;
          }
          else if (AMPLITUDE_MOD > 100)
          {
              AMPLITUDE_MOD = 100;
          }

          sprintf(disp_string,"Amplitude: %d",AMPLITUDE_MOD);
     }

     Update_Menu_Item(Menu_Selection,disp_string);
 }

 void Update_Menu_Item(int selection,char* str)
 {
     int y = selection*10;
     ST7735_FillRectangle(MENU_PAD,y,ST7735_WIDTH-MENU_PAD,9,MENU_BG); // clear numbers
     ST7735_WriteString(MENU_PAD, y, str, Font_7x10, MENU_FG,
                          MENU_BG); // rewrite it
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

//     if(ButtonLeftTime > 0)
//     {
//         if(currentTime > ButtonLeftTime + BUTTON_DELAY_mS)
//         {
//             if (HAL_GPIO_ReadPin(BUTTON_LEFT_PORT,BUTTON_LEFT_PIN) == 0)
//             {
//                 Backward_Menu();
//             }
//             ButtonLeftTime = 0;
//         }
//     }
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
//     if (GPIO_Pin == BUTTON_LEFT_PIN)
//     {
//         ButtonLeftTime = HAL_GetTick();
//     }

     if(GPIO_Pin == GPIO_PIN_1) //encoder channel 1 rising edge
     {
         uint32_t val1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
         uint32_t val2 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);

         if (val1 != EncoderState[0])
         {
             EncoderState[0] = val1;

             if (EncoderState[0] == 0)
             {
                 if (val2 == 1)
                 {
                     EncoderVal++;
                     Adjust_Parameter(1);
                 }
                 else
                 {
                     EncoderVal--;
                     Adjust_Parameter(-1);

                 }
             }
         }
         debug_printf(&HUART2,"Encoder val: %d\n\r",EncoderVal);
     }

     if (GPIO_Pin == GPIO_PIN_2) //encoder channel 1 rising edge
     {
         uint32_t val1 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2);
         uint32_t val2 = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);

          if (val1 != EncoderState[0])
          {
              EncoderState[0] = val1;

              if (EncoderState[0] == 0)
              {
                  if (val2 == 1)
                  {
                      EncoderVal--;
                      Adjust_Parameter(-1);
                  }
                  else
                  {
                      EncoderVal++;
                      Adjust_Parameter(1);
                  }
              }
          }
          debug_printf(&HUART2,"Encoder val: %d\n\r",EncoderVal);
     }
 }
