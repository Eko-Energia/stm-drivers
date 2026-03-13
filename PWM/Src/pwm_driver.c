/**
 * @file pwm_driver.c
 * @brief PWM input signal driver for PERLA project
 *
 * This module provides initialisation and update routines
 * for measuring PWM duty cycle using STM32 timer input capture
 * in PWM Input mode.
 *
 * The driver supports PWM input configured either on:
 *  - TIM Channel 1 (period on CH1, duty on CH2)
 *  - TIM Channel 2 (period on CH2, duty on CH1)
 *
 * To achieve a sensible reading timer frequency MUST be lower than that
 * of the signal to be red. Whenever possible it is recommended to use the
 * maximal possible counter period value. Calculated width will always be slightly
 * lower than the actual, it is caused by the time it takes for the signal to change from
 * high to low state, longer change causes accuracy loss.
 *
 * @author AGH EKO-ENERGIA
 * @author Andrzej Gondek
 */
#include "pwm_driver.h"
#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
/**

* @brief Set PWM pulse width as a fraction of the timer period.
*
* This function sets the PWM duty cycle for a specified timer channel by
* converting a normalized width value (0.0–1.0) into a timer compare value
* based on the current auto-reload register (ARR). The calculated compare
* value determines how long the PWM output stays high during one period.
*
* @param htim Pointer to the TIM HAL handle used for PWM generation.
* @param Channel Timer channel to update.
* @param width Normalized duty cycle in the range [0.0, 1.0].
  */
void PWM_setWidth(TIM_HandleTypeDef *htim, uint32_t Channel, float width)
{
	uint32_t pulseValue = (int)round((float)__HAL_TIM_GET_AUTORELOAD(htim)*width);
	__HAL_TIM_SET_COMPARE(htim,Channel,pulseValue);
}


/**
 * @brief Initialize PWM input signal structure and timer channel
 *
 * This function initializes the PWM_signal structure fields and
 * configures the selected timer input capture channel for PWM
 * input measurement.
 *
 * The timer must already be configured in PWM Input mode
 *
 * @param signal Pointer to PWM_signal structure to initialize
 * @param frequency Expected PWM signal frequency
 * @param isChannel1
 *        - true  : PWM input configured on TIM Channel 1
 *        - false : PWM input configured on TIM Channel 2
 * @param htim Pointer to the timer handle used for input capture
 */
void PWM_initialize(struct PWM_signal* signal,
                    int frequency,
                    bool isChannel1,
                    TIM_HandleTypeDef *htim)
{
    signal->width = 0.0f;
    signal->readFlag = false;
    signal->frequency = frequency;
    signal->icVal = 0;
    signal->ch1 = isChannel1;
    /* Configure input capture parameters */
    signal->sConfigIC.ICPolarity  = TIM_INPUTCHANNELPOLARITY_RISING;
    signal->sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    signal->sConfigIC.ICPrescaler = TIM_ICPSC_DIV8;
    signal->sConfigIC.ICFilter    = 0;
    if (isChannel1)
    {
        HAL_TIM_IC_ConfigChannel(htim, &signal->sConfigIC, TIM_CHANNEL_1);
    }
    else
    {
        HAL_TIM_IC_ConfigChannel(htim, &signal->sConfigIC, TIM_CHANNEL_2);
    }
}

/**
 * @brief Update PWM duty cycle measurement
 *
 * This function reads captured timer values and computes
 * the PWM duty cycle in percent.
 *
 * Behavior depends on the PWM input configuration:
 * - If PWM input is on Channel 1:
 *     - CH1 captures period
 *     - CH2 captures high time
 * - If PWM input is on Channel 2:
 *     - CH2 captures period
 *     - CH1 captures high time
 * Function is to be used inside the HAL_TIM_IC_CaptureCallback
 * after verifying whether the correct timer caused the interrupt.
 * The result is stored in PWM_signal::width as a percentage
 * value in the range 0–100.
 *
 * @param htim Pointer to the timer handle used for input capture
 * @param PWM Pointer to initialized PWM_signal structure
 */
void PWM_update(TIM_HandleTypeDef *htim, struct PWM_signal *PWM)
{
    if (PWM->ch1)
    {
        PWM->icVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        if (PWM->icVal != 0)
        {
            PWM->width =
                ((float)HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) * 100.0f)
                / (float)PWM->icVal;
        }
    }
    else
    {
        PWM->icVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

        if (PWM->icVal != 0)
        {
            PWM->width =
                ((float)HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) * 100.0f)
                / (float)PWM->icVal;
        }
    }
}
