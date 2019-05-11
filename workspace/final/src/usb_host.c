/*
 * usb_host.c
 *
 *  Created on: May 8, 2019
 *      Author: larry
 */


/**
 * Functions to control CH375 usb host SPI board
 */

#include "uart.h"
#include "usb_host.h"
#include "stm32f1xx_hal.h"
#include "debug_utils.h"

void USB_check_connection();
void USB_set_host_mode();
uint8_t USB_send_command(uint8_t,uint8_t);

void USB_Init()
{
    USB_check_connection();
    USB_set_host_mode();
}

void USB_check_connection()
{
    uint8_t test_val = 0x04;
    uint8_t result = USB_send_command(CH375_CMD_CHECK_EXIST,test_val);
    if (result == 255-test_val)
    {
        debug_printf(&HUART2,"USB board connected, %d\n",result);
    }
    else
    {
        debug_printf(&HUART2,"Error finding USB board: %d\n",result);
    }
}

void USB_set_host_mode()
{
    uint8_t result = USB_send_command(CH375_CMD_SET_USB_MODE,
            CH375_USB_MODE_HOST);

    //USB_send_command(0x15,0x07);

    if (result == CH375_CMD_RET_SUCCESS)
    {
        debug_printf(&HUART2,"USB host mode set, %d\n",result);
    }
    else
    {
        debug_printf(&HUART2,"Error setting USB host mode: %d\n",result);
    }

    HAL_Delay(100);

    // check if device is connected
    uint8_t command = 0x16;
    HAL_UART_Transmit(&HUART3,&command,1,0xFFFF);
    HAL_Delay(100);
    HAL_UART_Receive(&HUART3,&result,1,0xFFFF);

    if (result == CH375_USB_INT_CONNECT)
    {
        debug_printf(&HUART2,"USB device connected",0);

        //check what type of device
        result = USB_send_command(0x46,0x01); // get device descriptor
    }

}

uint8_t USB_send_command(uint8_t command, uint8_t arg)
{
    uint8_t rxBuffer = 0;

    if(HUART3.gState == HAL_UART_STATE_READY)
    {
        //uint8_t buffer[2] = {command,arg};
        uint8_t buffer = command;
        HAL_UART_Transmit(&HUART3,&buffer,1,0xFFFF);

        HAL_Delay(100); // wait for reply
        buffer = arg;
        HAL_UART_Transmit(&HUART3,&buffer,1,0xFFFF);

        HAL_Delay(100);
        HAL_UART_Receive(&HUART3,&rxBuffer,1,0xFFFF);
    }

    return rxBuffer;
}

void USB_read_data()
{
    uint8_t rxBuffer = 0;

    if(HUART3.gState == HAL_UART_STATE_READY)
    {
        uint8_t buffer = 0x28;
        HAL_UART_Transmit(&HUART3,&buffer,1,0xFFFF);

        HAL_Delay(100); // wait for reply
        HAL_UART_Receive_IT(&HUART3,&rxBuffer,1); // get length

        HAL_Delay(100); // wait.. may not be necessary
        uint8_t dataBuffer[rxBuffer];
        HAL_UART_Receive_IT(&HUART3,dataBuffer,rxBuffer);
    }
}

