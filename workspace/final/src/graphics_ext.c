/*
 * graphics_ext.c
 *
 *  Created on: Apr 27, 2019
 *      Author: larry
 */

#include <math.h>
#include "stm32f1xx_hal.h"
#include "st7735.h"
#include "sig_gen.h"
#include "graphics_ext.h"


void Display_Waveform(int x, int y, int x2, int y2)
 {
    // ensure first coordinates are smaller than second coordinates
    if (x > x2)
    {
        int xTemp = x;
        x = x2;
        x2 = xTemp;
    }
    if (y > y2)
    {
        int yTemp = y;
        y = y2;
        y2 = yTemp;
    }

    // clear region
    ST7735_FillRectangle(x,y,x2-x,y2-y,ST7735_BLACK);

    // draw outline
    Display_Unfilled_Rectangle(x,y,x2,y2,ST7735_WHITE);

    // draw grid lines
    int mid = ((y2-y)/2.0f) + y;
    Display_Dots(x,mid,x2,mid,ST7735_WHITE,4);

    // now draw actual waveform from buffer data

    // interpolate buffer to fit number of display points
    // may be necessary in future... currently don't use it for simplicity
    //int skip_size = NUM_SAMPLES/(ST7735_WIDTH-2);
    //int repeat_amount = (x2-x-2)/(NUM_SAMPLES*2);
    int repeat_amount = 1;
    int display_x = x;
    int wave_height = (y2-y-2);
    int prev_y = -1;

    while (display_x < x2)
    {

        for (int i = 0; i < NUM_SAMPLES; i++)
        {
            int display_y;
            switch(Selected_Signal)
            {
                 case Sine:
                     display_y = (SineBuffer[i]/(AMPLITUDE*2.0f)) * wave_height
                         + y + 1;
                     break;
                 case Triangle:
                     display_y = (TriangleBuffer[i]/(AMPLITUDE*2.0f)) * wave_height
                         + y + 1;
                     break;
                 case Square:
                     display_y = (SquareBuffer[i]/(AMPLITUDE*2.0f)) * wave_height
                         + y + 1;
                     break;
                 default:
                     display_y = 0;
                     break;
            }

            if (prev_y > 0)
            {
                 if (display_y > prev_y)
                 {
                     // connect the previous and current point vertically (up)
                     ST7735_FillRectangle(display_x,prev_y,1,display_y-prev_y,ST7735_CYAN);
                 }
                 else if (display_y == prev_y)
                 {
                     // current point is on the same line, doesn't need to be connected
                     ST7735_DrawPixel(display_x,display_y,ST7735_CYAN);
                 }
                 else
                 {
                     // connect the previous and current point vertically (down)
                     ST7735_FillRectangle(display_x,display_y,1,prev_y-display_y,ST7735_CYAN);
                 }
            }
            else // this is the first point, no connection needed
            {
                ST7735_DrawPixel(display_x,display_y,ST7735_CYAN);
            }

            // increment x value and store y value for next loop
            display_x += repeat_amount;
            if (display_x >= x2)
            {
                break;
            }

            prev_y = display_y;
        }
    }


 }

void Display_Line(int x,int y,int x2,int y2,uint16_t color)
{
    int w = 0;

    if (y > y2)
    {
        w = y - y2;
    }
    else
    {
        w = y2 - y;
    }

    float slope = ((float)y2 - (float)y)/((float)x2 - (float)x);

    if (x > x2)
    {
        int temp = x;
        x = x2;
        x2 = temp;
    }

    // Straight lines can use the rectangle function
    if (x == x2)
    {
        ST7735_FillRectangle(x,y,1,w,color);
        return;
    }
    if (y == y2)
    {
        ST7735_FillRectangle(x,y,x2-x,1,color);
    }


    if (w < x2 - x)
    {
        for (int i = x; i <= x2; i++)
        {
            int j = (int)round(slope*(float)i + (float)y);

            ST7735_DrawPixel(i,j,color);
        }
    }
//    else
//    {
//        for (int j = y; j <= y2; j++)
//        {
//            int j = (int)round(slope*(float)i + (float)y);
//
//            ST7735_DrawPixel(i,j,color);
//        }
//    }


}

void Display_Unfilled_Rectangle(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2,uint16_t color)
{
     // horizontal
     ST7735_FillRectangle(x,y,x2-x,1,color);
     ST7735_FillRectangle(x,y2,x2-x,1,color);

     // vertical
     ST7735_FillRectangle(x,y,1,y2-y,color);
     ST7735_FillRectangle(x2,y,1,y2-y+1,color);
}

void Display_Dots(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2,
        uint16_t color, uint16_t density)
{
    int count = 0;

    for (int yOut = y; yOut <= y2; yOut++)
    {
        for (int xOut = x; xOut <= x2; xOut++)
        {
            if (count%density == 0) // every other time
            {
                ST7735_DrawPixel(xOut,yOut,color);
            }
            count++;

        }
    }
}

// GRAPHICS DEMOS

void Graphics_Demo()
{
    ST7735_FillScreen(ST7735_BLACK);

    int prev_i = ST7735_HEIGHT;
    for (int i = ST7735_HEIGHT; i > 0; i-=6)
    {
        ST7735_WriteString(0,prev_i,"                       ",Font_7x10,
                    ST7735_WHITE,ST7735_BLACK); // clear prev text
        ST7735_WriteString(0,i,"INITIALIZING DEMO",Font_7x10,
                            ST7735_WHITE,ST7735_BLACK); // update text

        HAL_Delay(100); // delay to slow down animation

        prev_i = i;
    }

    ST7735_WriteString(0,0,"INITIALIZING DEMO.",Font_7x10,
                                ST7735_WHITE,ST7735_BLACK);
    HAL_Delay(100);
    ST7735_WriteString(0,0,"INITIALIZING DEMO..",Font_7x10,
                                ST7735_WHITE,ST7735_BLACK);
    HAL_Delay(100);
    ST7735_WriteString(0,0,"INITIALIZING DEMO...",Font_7x10,
                                ST7735_WHITE,ST7735_BLACK);
    HAL_Delay(1000);

    ST7735_FillScreen(ST7735_BLACK);

    ST7735_WriteString(0,0,"Draw Lines",Font_7x10,
                            ST7735_WHITE,ST7735_BLACK); // clear prev text

    Display_Line(80,0,80,ST7735_HEIGHT,ST7735_CYAN);

    //Display_Line(20,0,ST7735_WIDTH-20,ST7735_HEIGHT,ST7735_CYAN);

    Display_Line(0,40,ST7735_WIDTH,ST7735_HEIGHT-40,ST7735_CYAN);

    Display_Line(0,0,ST7735_WIDTH,ST7735_HEIGHT,ST7735_CYAN);

    Display_Line(0,ST7735_HEIGHT-40,ST7735_WIDTH,40,ST7735_CYAN);

    Display_Line(0,64,ST7735_WIDTH,64,ST7735_CYAN);

    Display_Line(0,ST7735_HEIGHT,ST7735_WIDTH,0,ST7735_CYAN);

    HAL_Delay(2000);

    ST7735_FillScreen(ST7735_BLACK);

    ST7735_WriteString(0,0,"Dotted Lines",Font_7x10,ST7735_CYAN,ST7735_BLACK);

    // VERTICAL LINES
    Display_Dots(0,0,0,ST7735_HEIGHT,ST7735_MAGENTA,2);
    Display_Dots(80,0,80,ST7735_HEIGHT,ST7735_MAGENTA,2);
    Display_Dots(ST7735_WIDTH-1,0,ST7735_WIDTH-1,ST7735_HEIGHT,ST7735_MAGENTA,2);

    // HORZ LINES
    Display_Dots(0,0,ST7735_WIDTH,0,ST7735_MAGENTA,2);
    Display_Dots(0,64,ST7735_WIDTH,64,ST7735_MAGENTA,2);
    Display_Dots(0,ST7735_HEIGHT-1,ST7735_WIDTH,ST7735_HEIGHT-1,ST7735_MAGENTA,2);

    HAL_Delay(2000);

    ST7735_WriteString(0,0,"                     ",Font_7x10,ST7735_CYAN,ST7735_BLACK);


    Display_Dots(0,0,ST7735_WIDTH,ST7735_HEIGHT,ST7735_MAGENTA,2);
    HAL_Delay(1000);

    Display_Dots(0,0,ST7735_WIDTH,ST7735_HEIGHT,ST7735_CYAN,4);
    HAL_Delay(1000);

    Display_Dots(0,0,ST7735_WIDTH,ST7735_HEIGHT,ST7735_YELLOW,5);
    HAL_Delay(1000);

    ST7735_FillScreen(ST7735_BLACK);

    ST7735_FillScreen(ST7735_YELLOW);
    Display_Dots(0,0,ST7735_WIDTH,ST7735_HEIGHT,ST7735_BLACK,11);

    HAL_Delay(1000);

    ST7735_FillScreen(ST7735_WHITE);

    ST7735_WriteString(0,0,"Unfilled Rectangles",Font_7x10,ST7735_BLACK,ST7735_WHITE);
    HAL_Delay(2000);


    int x = 0;
    int y = 0;
    while(x <= ST7735_HEIGHT/2)
    {

        Display_Unfilled_Rectangle(x,y,ST7735_WIDTH - x,ST7735_HEIGHT - y,
                ST7735_RED);

        x++;
        y++;

        if (x > ST7735_HEIGHT/2)
        {
            break;
        }

        Display_Unfilled_Rectangle(x,y,ST7735_WIDTH - x,ST7735_HEIGHT - y,
                        ST7735_GREEN);

        x++;
        y++;

        if (x > ST7735_HEIGHT/2)
        {
            break;
        }

        Display_Unfilled_Rectangle(x,y,ST7735_WIDTH - x,ST7735_HEIGHT - y,
                        ST7735_BLUE);

        x++;
        y++;

        HAL_Delay(200);
    }

    HAL_Delay(1000);

    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(0,0,"Waveform Displays",Font_7x10,ST7735_CYAN,ST7735_BLACK);

    enum SignalType temp = Selected_Signal;

    Selected_Signal = Sine;
    Display_Waveform(0,20,ST7735_WIDTH-1,40);

    Selected_Signal = Triangle;
    Display_Waveform(0,60,ST7735_WIDTH-1,80);

    Selected_Signal = Square;
    Display_Waveform(0,100,ST7735_WIDTH-1,120);

    Selected_Signal = temp;

    HAL_Delay(2000);


}

// provided by ST7725 Library

void graphics_test() {
    // Check border
//    ST7735_FillScreen(ST7735_BLACK);
//
//    for(int x = 0; x < ST7735_WIDTH; x++) {
//        ST7735_DrawPixel(x, 0, ST7735_RED);
//        ST7735_DrawPixel(x, ST7735_HEIGHT-1, ST7735_RED);
//    }
//
//    for(int y = 0; y < ST7735_HEIGHT; y++) {
//        ST7735_DrawPixel(0, y, ST7735_RED);
//        ST7735_DrawPixel(ST7735_WIDTH-1, y, ST7735_RED);
//    }
//
//    HAL_Delay(3000);

    // Check fonts
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(0, 0, "Font_7x10, red on black, lorem ipsum dolor sit amet", Font_7x10, ST7735_RED, ST7735_BLACK);
    ST7735_WriteString(0, 3*10, "Font_11x18, green, lorem ipsum", Font_11x18, ST7735_GREEN, ST7735_BLACK);
    ST7735_WriteString(0, 3*10+3*18, "Font_16x26", Font_16x26, ST7735_BLUE, ST7735_BLACK);
    HAL_Delay(2000);

    // Check colors
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_WriteString(0, 0, "BLACK", Font_11x18, ST7735_WHITE, ST7735_BLACK);
    HAL_Delay(500);

    ST7735_FillScreen(ST7735_BLUE);
    ST7735_WriteString(0, 0, "BLUE", Font_11x18, ST7735_BLACK, ST7735_BLUE);
    HAL_Delay(500);

    ST7735_FillScreen(ST7735_RED);
    ST7735_WriteString(0, 0, "RED", Font_11x18, ST7735_BLACK, ST7735_RED);
    HAL_Delay(500);

    ST7735_FillScreen(ST7735_GREEN);
    ST7735_WriteString(0, 0, "GREEN", Font_11x18, ST7735_BLACK, ST7735_GREEN);
    HAL_Delay(500);

    ST7735_FillScreen(ST7735_CYAN);
    ST7735_WriteString(0, 0, "CYAN", Font_11x18, ST7735_BLACK, ST7735_CYAN);
    HAL_Delay(500);

    ST7735_FillScreen(ST7735_MAGENTA);
    ST7735_WriteString(0, 0, "MAGENTA", Font_11x18, ST7735_BLACK, ST7735_MAGENTA);
    HAL_Delay(500);

    ST7735_FillScreen(ST7735_YELLOW);
    ST7735_WriteString(0, 0, "YELLOW", Font_11x18, ST7735_BLACK, ST7735_YELLOW);
    HAL_Delay(500);

    ST7735_FillScreen(ST7735_WHITE);
    ST7735_WriteString(0, 0, "WHITE", Font_11x18, ST7735_BLACK, ST7735_WHITE);
    HAL_Delay(500);
}

