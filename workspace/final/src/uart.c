/**
* @file uart.c
* @author Larry Hixenbaugh
* @date Feb 23, 2019
* @brief Functions to initialize and respond to messages onUART1 and UART2.
*
* Implements interrupts on UART1 and contains functions to respond to serial
* commands that control the counter value and behavior.
*/

#include <stdio.h>
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "debug_utils.h"

#define HEADER_SIZE (5)

// value for RxState
#define READ_HEADER (0)
#define READ_SHORT_MESSAGE (1)
#define READ_LONG_MESSAGE (2)

UART_HandleTypeDef HUART1;
UART_HandleTypeDef HUART2;
UART_HandleTypeDef HUART3;

uint8_t Buffer[8];

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // check if caller is UART1 or 2
    // example code from module 4 discussion board:

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance==USART1)
    {
        /* Peripheral clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    else if(huart->Instance==USART2)
    {
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */

        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    else if(huart->Instance == USART3)
    {
        __HAL_RCC_USART3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /**USART3 GPIO Configuration
        PB10     ------> USART3_TX
        PB11     ------> USART3_RX
        */

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void UART2_Init(void)
{
    // based on example from https://visualgdb.com/tutorials/arm/stm32/uart/hal/
    HUART2.Instance = USART2;
    HUART2.Init.BaudRate = 115200;
    HUART2.Init.WordLength = UART_WORDLENGTH_8B;
    HUART2.Init.StopBits = UART_STOPBITS_1;
    HUART2.Init.Parity = UART_PARITY_NONE;
    HUART2.Init.Mode = UART_MODE_TX_RX;
    HUART2.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    HUART2.gState = HAL_UART_STATE_RESET;

    if (HAL_UART_Init(&HUART2) != HAL_OK)
    {
        Error_Handler();
    }
}

void UART3_Init(void)
{
    // based on example from https://visualgdb.com/tutorials/arm/stm32/uart/hal/
    HUART3.Instance = USART3;
    HUART3.Init.BaudRate = 115200;
    HUART3.Init.WordLength = UART_WORDLENGTH_8B;
    HUART3.Init.StopBits = UART_STOPBITS_1;
    HUART3.Init.Parity = UART_PARITY_NONE;
    HUART3.Init.Mode = UART_MODE_TX_RX;
    HUART3.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    HUART3.gState = HAL_UART_STATE_RESET;

    if (HAL_UART_Init(&HUART3) != HAL_OK)
    {
        Error_Handler();
    }

    // Enable Interrupt
    HAL_NVIC_SetPriority(USART3_IRQn,4,0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
}

void UART1_Init(void)
{
    HUART1.Instance = USART1;
    HUART1.Init.BaudRate = 115200;
    HUART1.Init.WordLength = UART_WORDLENGTH_8B;
    HUART1.Init.StopBits = UART_STOPBITS_1;
    HUART1.Init.Parity = UART_PARITY_NONE;
    HUART1.Init.Mode = UART_MODE_TX_RX;
    HUART1.Init.HwFlowCtl = UART_HWCONTROL_NONE;

    HUART1.gState = HAL_UART_STATE_RESET;

    if (HAL_UART_Init(&HUART1) != HAL_OK)
    {
        Error_Handler();
    }

    // Enable Interrupt
    HAL_NVIC_SetPriority(USART1_IRQn,4,0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    HAL_UART_Receive_IT(&HUART1, Buffer, HEADER_SIZE);
}


void send_ack(void)
{
    uint8_t ack[7] = {0x1A, 0xCF, 0xFC, 0x1D, 0x07, 0xFF, 0xCC};
    HAL_UART_Transmit_IT(&HUART1, ack, sizeof(ack));
}

void send_nack(void)
{
    uint8_t nack[7] = {0x1A, 0xCF, 0xFC, 0x1D, 0x07, 0xFE, 0xCD};
    HAL_UART_Transmit_IT(&HUART1, nack, sizeof(nack));
}



volatile int RxState = READ_HEADER;

void read_header(void)
{
    if (Buffer[0] == 0x1A &&
                    Buffer[1] == 0xCF &&
                    Buffer[2] == 0xFC &&
                    Buffer[3] == 0x1D)
    {
        // found all sync bytes, check message length
        if (Buffer[4] == 7)
        {
            // short message
            RxState = READ_SHORT_MESSAGE;
            HAL_UART_Receive_IT(&HUART1, &Buffer[5], 2); // listen again
        }
        else if (Buffer[4] == 8)
        {
            // long message
            RxState = READ_LONG_MESSAGE;
            HAL_UART_Receive_IT(&HUART1, &Buffer[5], 3); // listen again
        }
        else
        {
            // invalid message length
            send_nack();
            HAL_UART_Receive_IT(&HUART1, Buffer, HEADER_SIZE); // listen again
        }
    }
    else
    {
        HAL_UART_Receive_IT(&HUART1, Buffer, HEADER_SIZE); // listen again
    }
}

uint8_t calculate_checksum()
{
    uint8_t checksum = 0;
    for (int i = 0; i < Buffer[4]-1; i++) // Buffer[4] contains the message length
    {
        checksum ^= Buffer[i];
    }

    return checksum;
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
////    if (RxState == READ_HEADER)
////    {
////        read_header();
////    }
////    else if (RxState == READ_SHORT_MESSAGE)
////    {
////        if (calculate_checksum() == Buffer[6])
////        {
////            // execute command based on Buffer
////            run_command(Buffer[5]);
////        }
////        else
////        {
////            // invalid checksum
////            send_nack();
////        }
////
////        RxState = READ_HEADER; // start listening for header again
////        HAL_UART_Receive_IT(&HUART1, Buffer, HEADER_SIZE); // listen again
////    }
////    else if (RxState == READ_LONG_MESSAGE)
////    {
////        if (calculate_checksum() == Buffer[7])
////        {
////            run_command_with_argument(Buffer[5],Buffer[6]);
////        }
////        else
////        {
////            // invalid checksum
////            send_nack();
////        }
////
////        RxState = READ_HEADER; // start listening for header again
////        HAL_UART_Receive_IT(&HUART1, Buffer, HEADER_SIZE); // listen again
////    }
//}

//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
//{
//    send_nack();
//
//    RxState = READ_HEADER;
//    HAL_UART_Receive_IT(&HUART1, Buffer, HEADER_SIZE); // listen again
//}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    // currently unused
}


