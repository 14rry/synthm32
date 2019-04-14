/**
* @file debug_utils.h
* @author Larry Hixenbaugh
* @date Feb 23, 2019
* @brief Serial/UART debug print tool and general error handler
*/

#ifndef DEBUG_UTILS_H_
#define DEBUG_UTILS_H_

#include "stm32f1xx_hal.h"

#define DEBUG_UTILS_ENABLE   (1) /**< set to 1 to enable, anything else will
                                    disable */
#define DEBUG_UTILS_BUFF_LEN (256) /**< max internal buffer size used to hold
                                    string, change as required */
#define Error_Handler() Error_Handler_caller(__func__) /**< Macro for error
                            handling in conjunction with above function */

/**
 * @brief Handle errors with an infinite loop and a breakpoint
 * @param the name of the function that called the error handler
 */
void Error_Handler_caller(char const * caller_name);

/**
 * @brief prints a string and value to the uart
 * @param hUart Handle for the desired uart
 * @param fmt string to be printed
 */
void debug_printf(UART_HandleTypeDef *hUart, char *fmt, ...);

#endif /* DEBUG_UTILS_H_ */
