/*
 * lcd_driver.h
 *
 *  Created on: Apr 6, 2019
 *      Author: larry
 */

#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_

#include "stm32f1xx_hal.h"

#define SPI_BASE_ADDR (0x40031000)
#define RCC_BASE_ADDR (0x40021000)
#define RCC_APB2_CLK_ENABLE_OFFSET (0x18)

// SPI all on port A
#define MISO_PIN (GPIO_PIN_6)
#define MOSI_PIN (GPIO_PIN_7)
#define SCK_PIN (GPIO_PIN_5)
#define LED_PIN (GPIO_PIN_1)
#define SS_PIN (GPIO_PIN_10)
#define RESET_PIN (GPIO_PIN_9)

#define DATA_CONTROL_PIN (GPIO_PIN_8)

// global variables
SPI_HandleTypeDef hSPI;

// function prototypes
void SPI_Init();
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi);
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

void LCDWrite(uint8_t, uint8_t*, int);
void LCDWrite16(uint8_t, uint16_t*, int);
void ST7735_Write_Command(uint8_t);
void Loopback_Test(uint8_t Data_Control_n, uint8_t* data, int numBytes);

#endif /* LCD_DRIVER_H_ */
