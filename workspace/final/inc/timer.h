/*
 * timer.h
 *
 *  Created on: Mar 5, 2019
 *      Author: larry
 */

#ifndef TIMER_H_
#define TIMER_H_

#define TIM2_LCD_PULSE_DIM (300) /*< Initialize with 25% duty cycle */
#define TIM2_LCD_Pulse_DIM_2 (700)
#define TIM2_LCD_PULSE_BRIGHT (1279)

TIM_HandleTypeDef htim_periodic; // handle for periodic timer
TIM_HandleTypeDef htim_encoder; // handle for encoder timer
TIM_HandleTypeDef htim_pwm; // handle for PWM timer

// GLOBALS
volatile int PeriodicTimerFlag; /*< Flag that is raised when the periodic timer ellapses */

// FUNCTION PROTOTYPES
/**
 * @brief Initialize periodic, encoder, and PWM timers for DAC generation
 */
void Timer_Init();

/**
 * @brief override weak HAL function, initialize GPIO/clock for periodic timer
 * @param htim handle to periodic timer
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim);

/**
 * @brief override weak HAL function, intialize GPIO/clock for encoder
 * @param htim handle to encoder timer
 */
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* htim);

/**
 * @brief override weak HAL function, initialize PWM timer
 * @param htim handle to PWM timer
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim);

/**
 * @brief Callback function linked to periodic timer interrupt trigger
 *
 * Raises periodic timer flag that tells main loop to update analog voltage level
 * @param htim handle to periodic timer
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);

/**
 * @brief Initialize periodic timer for use with DAC
 */
void Initialize_Periodic_Timer();

/**
 * @brief Initialize encoder timer to read rotary encoder values
 */
void Initialize_Encoder_Timer();

/**
 * @brief Initialize PWM timer as RC circuit input to generate analog voltages
 */
void Initialize_PWM_Timer();

/**
 * @brief Poll value of rotary encoder. Perform action if value changed.
 *
 * Changes frequency of audio signal.
 */
void Check_Encoder();

#endif /* TIMER_H_ */
