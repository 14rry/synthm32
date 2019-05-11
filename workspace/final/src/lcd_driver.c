/*
 * lcd_driver.c
 *
 *  Created on: Apr 6, 2019
 *      Author: larry
 */

#include <string.h>
#include "lcd_driver.h"
#include "stm32f1xx_hal.h"
#include "debug_utils.h"
#include "uart.h"
#include "timer.h"



/*
 * INITIALIZATION FUNCTIONS
 */
void SPI_Init()
{
    hSPI.Instance = SPI1;

    hSPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hSPI.Init.Direction = SPI_DIRECTION_1LINE;
    hSPI.Init.CLKPhase = SPI_PHASE_1EDGE;
    hSPI.Init.CLKPolarity = SPI_POLARITY_LOW;
    hSPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hSPI.Init.DataSize = SPI_DATASIZE_8BIT;
    hSPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hSPI.Init.NSS = SPI_NSS_SOFT;
    hSPI.Init.TIMode = SPI_TIMODE_DISABLE;
    hSPI.Init.CRCPolynomial = 10;
    hSPI.Init.Mode = SPI_MODE_MASTER;

    if (!HAL_SPI_Init(&hSPI) == HAL_OK)
    {
        Error_Handler();
    }

    __HAL_SPI_ENABLE(&hSPI);
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    //__HAL_RCC_AFIO_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = MOSI_PIN | SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = MISO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SS_PIN | DATA_CONTROL_PIN | RESET_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
 * LCD HELPER FUNCTIONS
 */

void Loopback_Test(uint8_t Data_Control_n, uint8_t* data, int numBytes)
{

    uint8_t rxData[numBytes];

    // set data_control_n line
    HAL_GPIO_WritePin(GPIOA, DATA_CONTROL_PIN, Data_Control_n & 1); // &1 ensures only one bit is sent

    // assert SPI slave select (active low)
    HAL_GPIO_WritePin(GPIOA, SS_PIN, GPIO_PIN_RESET);

    // transmit data
    HAL_SPI_TransmitReceive(&hSPI, data, rxData, numBytes, HAL_MAX_DELAY);

    // de-assert SPI slave select
    HAL_GPIO_WritePin(GPIOA, SS_PIN, GPIO_PIN_SET);

    // print result to serial line
    debug_printf(&HUART2, "Loopback Rx: %d\n\r",rxData[0]);

}

void LCDWrite(uint8_t Data_Control_n, uint8_t* data, int numBytes)
{
    // set data_control_n line
    HAL_GPIO_WritePin(GPIOA, DATA_CONTROL_PIN, Data_Control_n & 1); // &1 ensures only one bit is sent

    // assert SPI slave select (active low)
    HAL_GPIO_WritePin(GPIOA, SS_PIN, GPIO_PIN_RESET);

    // transmit data
    HAL_SPI_Transmit(&hSPI, data, numBytes, HAL_MAX_DELAY);

    // de-assert SPI slave select
    HAL_GPIO_WritePin(GPIOA, SS_PIN, GPIO_PIN_SET);

}

void LCDWrite16(uint8_t Data_Control_n, uint16_t *data, int numWords)
{
    // set data_control_n line
    HAL_GPIO_WritePin(GPIOA, DATA_CONTROL_PIN, Data_Control_n & 1); // &1 ensures only one bit is sent

    // assert SPI slave select (active low)
    HAL_GPIO_WritePin(GPIOA, SS_PIN, GPIO_PIN_RESET);

    // split data into 8 bit values, swap MSBs, transmit 8 bits at a time

    uint8_t data8bit[numWords*2];
    for (int i = 0; i < numWords; i++)
    {
        data8bit[(i*2)+1] = (uint8_t)*data & 0xFF;
        data8bit[(i*2)] = (*data) >> 8;

        data++;
    }

    HAL_SPI_Transmit(&hSPI, data8bit, numWords*2, HAL_MAX_DELAY);


    // de-assert SPI slave select
    HAL_GPIO_WritePin(GPIOA, SS_PIN, GPIO_PIN_SET);
}

void ST7735_Write_Command(uint8_t command)
{
    LCDWrite(0x0,&command,1);
}


void Toggle_LCD_Brightness()
{
    // toggle LCD backlight brightness
    if (htim_pwm.Instance->CCR2 == TIM2_LCD_PULSE_DIM)
    {
        __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, TIM2_LCD_Pulse_DIM_2);
    }
    else if (htim_pwm.Instance->CCR2 == TIM2_LCD_Pulse_DIM_2)
    {
        __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, TIM2_LCD_PULSE_BRIGHT);
    }
    else
    {
        __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, TIM2_LCD_PULSE_DIM);
    }
}

/*
 * GPIO INTERRUPT CALLBACK FUNCTION
 */
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//    // blue onboard button
//    if (GPIO_Pin == GPIO_PIN_13)
//    {
//        // toggle LCD backlight brightness
//        if (htim_pwm.Instance->CCR2 == TIM2_LCD_PULSE_DIM)
//        {
//            __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, TIM2_LCD_PULSE_BRIGHT);
//        }
//        else
//        {
//            __HAL_TIM_SET_COMPARE(&htim_pwm, TIM_CHANNEL_2, TIM2_LCD_PULSE_DIM);
//        }
//    }
//}
