/*
 * sig_gen.h
 *
 *  Created on: Mar 31, 2019
 *      Author: larry
 */

#ifndef SIG_GEN_H_
#define SIG_GEN_H_

#include "stm32f1xx_hal.h"

/**
 * GPIO Definitinos
 */

/* external LED GPIO pins and ports, displays 3-bit binary value
 *  each LED is on a separate port due to the wiring schematic */
#define ULED1_PORT GPIOB /**< ULED1_PORT GPIOA external LED LSB */
#define ULED2_PORT GPIOC /**< ULED2_PORT GPIOC external LED middle bit */
#define ULED3_PORT GPIOB /**< ULED3_PORT GPIOB external LED MSB */

#define ULED1 GPIO_PIN_8 /**< ULED1 GPIO_PIN_9 external LED LSB */
#define ULED2 GPIO_PIN_7 /**< ULED2 GPIO_PIN_7 external LED middle bit */
#define ULED3 GPIO_PIN_6 /**< ULED3 GPIO_PIN_6 external LED MSB */

/* external momentary push-buttons, active low used to control LED display value
 * Each button is on the same GPIO port
 */
#define BUTTON_PORT GPIOC /**< BUTTON_PORT GPIOC all buttons on the same port */

#define BUTTON1 GPIO_PIN_3 /**< BUTTON1 GPIO_PIN_3 */
#define BUTTON2 GPIO_PIN_2 /**< BUTTON2 GPIO_PIN_2 */
#define BUTTON3 GPIO_PIN_4 /**< BUTTON3 GPIO_PIN_4 */

/**
 * Audio Freuquency Definitions
 */
#define G0 (158) /**< ARR for G0 */
#define A0 (141) /**< ARR for A0 */
#define B0 (126) /**< ARR for B0 */
#define C1 (118) /**< ARR for C1 */
#define D1 (105) /**< ARR for D1 */
#define E1 (94)  /**< ARR for E1 */
#define F1 (88)  /**< ARR for F1 */
#define G1 (79)  /**< ARR for G1 */
#define A1 (70)  /**< ARR for A1 */
#define B1 (62)  /**< ARR for B1 */
#define C2 (59)  /**< ARR for C2 */
#define D2 (52)  /**< ARR for D2 */
#define E2 (46)  /**< ARR for E2 */
#define F2 (44)  /**< ARR for F2 */
#define G2 (39)  /**< ARR for G2 */

#define SCALE_LENGTH (15) /**< The number of values in the predefined scale */

#define NUM_SAMPLES (32) /**< Number of analog voltages to calcaulte for each waveform */
#define AMPLITUDE (500) /**< related to pulse width for PWM signal. Affects analog signal amplitude */

#define ATTACK_mS (1000)
#define DECAY_mS (100)
#define SUSTAIN_mS (500)
#define RELEASE_mS (1000)

// globals
int SineBuffer[NUM_SAMPLES]; /**< 32 sample buffer containing sine wave values */
int TriangleBuffer[NUM_SAMPLES]; /**< 32 sample buffer containing traingle wave values */
int SquareBuffer[NUM_SAMPLES]; /**< 32 sample buffer containing square wave values */
int SignalIndex; /**< current index in the waveform buffer */


enum SignalType
{
    Sine,
    Triangle,
    Square
};
enum SignalType Selected_Signal;

// function prototypes
/**
 * @brief Callback function linked to periodic timer interrupt trigger
 *
 * Raises periodic timer flag that tells main loop to update analog voltage level
 * @param htim handle to periodic timer
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim);

/**
 * @brief calculate the waveform buffers and initialize the timer values
 */
void Initialize_Sig_Gen();

/**
 * @brief called by initialize function, calculates buffers for three waveforms
 */
void Calculate_Buffers();

 /**
  * @brief Increases frequency up to the next note in the C Major scale
  */
void Increase_Note();

/**
 * @brief Decreases frequency down to the next lower note in the C major scale
 */
void Decrease_Note();

/**
 * @brief check if periodic timer ellapsed, if so go to the next value in the
 * waveform buffer
 */
void Update_Output_Signal();

void Change_Selected_Signal();

void Note_On(uint);
void Note_Off(void);


#endif /* SIG_GEN_H_ */
