/**
* @file debug_utils.c
* @author Larry Hixenbaugh
* @date Feb 23, 2019
* @brief Serial/UART debug print tool and general error handler
*/

  #include "debug_utils.h"
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>
  /**
    * @brief Debug Print Function
    * @param handle to HAL Uart to use
    * @param variable length formated string - similar to printf
    * @retval None
    */
  void debug_printf(UART_HandleTypeDef *hUart, char *fmt, ...)
  {
    #if DEBUG_UTILS_ENABLE == 1
        if(hUart->gState == HAL_UART_STATE_READY)
        {
            char buffer[DEBUG_UTILS_BUFF_LEN];
            //use variable argument helper functions to process the format string
            va_list va;
            va_start(va, fmt);
            vsprintf(buffer, fmt, va);
            va_end(va);
            HAL_UART_Transmit(hUart, (uint8_t*)buffer,strlen(buffer),0xFFFF);
        }
    #endif
}

  /**
   * @brief Handle errors with an infinite loop and a breakpoint
   * @param the name of the function that called the error handler
   */
  void Error_Handler_caller(char const * caller_name)
  {
      while(1); // do nothing.. set this as a breakpoint for debugging
  }
