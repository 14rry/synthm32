/**
* @file uart.h
* @author Larry Hixenbaugh
* @date Feb 23, 2019
* @brief Functions to initialize and respond to messages onUART1 and UART2.
*
* Implements interrupts on UART1 and contains functions to respond to serial
* commands that control the counter value and behavior.
*/

#ifndef UART_H_
#define UART_H_

UART_HandleTypeDef HUART1; /**< UART_HandleTypeDef HUART1 */
UART_HandleTypeDef HUART2; /**< UART_HandleTypeDef HUART2 */

/**
 * @brief Initializes GPIO pins for UART1 or 2
 *
 * Replaces a 'weak' function in the HAL library. Based on example code from
 * module 4 discussion board.
 * @param huart The UART_HandleTypeDef of the uart to be initialized
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart);

/**
 * @brief Populates the UART_HandleTypeDef with settings for UART1
 *
 * Specifies values such as baud rate, stopbits, parity, etc.
 * Also sets up interrupts to handle UART1 communication.
 * Partly based on example from
 * https://visualgdb.com/tutorials/arm/stm32/uart/hal/
 *
 * @return The newly created UART_HandleTypeDef for UART1
 */
UART_HandleTypeDef UART1_Init(void);

/**
 * @brief Popluates the UART_HandleTypeDef with settings for UART2
 *
 * Specifies values such as baud rate, stopbits, parity, etc.
 * Partly based on example from
 * https://visualgdb.com/tutorials/arm/stm32/uart/hal/
 * @return
 */
UART_HandleTypeDef UART2_Init(void);

/**
 * @brief callback handler for successful UART receive
 *
 * Consist of a state machine that first looks for a message header,
 * then parses the header and verifies the checksum, and finally runs the
 * appropriate command. Sends back an acknowledge message if everything worked.
 * Otherwise sends back a NACK message.
 *
 * @param huart Pointer to the UART handle that triggered the interrupt
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

/**
 * @brief Callback for UART receive errors
 *
 * Most common error seems to be timeout if not enough bits get sent.
 * Responds with a nack and then sets up to continue listening.
 * @param huart Pointer to the UART handle that triggered the interrupt
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

/**
 * @brief Callback gets called when UART transmit occurs
 *
 * Currently function is empty and does nothing. May be used in the future.
 * @param huart Pointer to the UART handle that triggered the interrupt
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

#endif /* UART_H_ */
