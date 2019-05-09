/*
 * graphics_ext.h
 *
 *  Created on: Apr 27, 2019
 *      Author: larry
 */

#ifndef GRAPHICS_EXT_H_
#define GRAPHICS_EXT_H_

void Display_Waveform(int x, int y, int x2, int y2);
void Display_Unfilled_Rectangle(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t color);
void Display_Dots(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2,
        uint16_t color,uint16_t density);
void Graphics_Demo();
void graphics_test();
void Display_Line(int x,int y,int x2,int y2,uint16_t color);
#endif /* GRAPHICS_EXT_H_ */
