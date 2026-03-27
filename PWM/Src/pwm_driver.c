/**
 * @file pwm_driver.c
 * @brief PWM signal driver for PERLA project
 *
 * This module provides initialisation and update routines
 * for measuring PWM duty cycle using STM32 timer input capture
 * in PWM Input mode and generating said signal.
 *
 * The driver supports PWM input configured either on:
 *  - TIM Channel 1 (period on CH1, duty on CH2)
 *  - TIM Channel 2 (period on CH2, duty on CH1)
 *
 * To achieve a sensible reading timer frequency MUST be lower than that
 * of the signal to be red. Whenever possible it is recommended to use the
 * maximal possible counter period value. Calculated duty will always be slightly
 * lower than the actual, it is caused by the time it takes for the signal to change from
 * high to low state, longer change causes accuracy loss.
 *
 * @author AGH EKO-ENERGIA
 * @author Andrzej Gondek
 */
#include "pwm_driver.h"
#include "main.h"
#include <math.h>
#include <stdbool.h>
/**
 * @brief Set PWM pulse duty as a percentage of the timer period.
 *
 * This function sets the PWM duty cycle for a specified timer channel by
 * converting a duty value (0.0–100.0) into a timer compare value
 * based on the current auto-reload register (ARR). The calculated compare
 * value determines how long the PWM output stays high during one period.
 *
 * @param PWM Pointer to PWM_signal structure containing timer handle, channel, and duty cycle.
 * @param duty PWM duty value from [0.0, 100.0] to be set.
 */
void PWM_Out_setDuty(struct PWM_Out_signal *PWM,float duty)
{
	uint32_t pulseValue = (int)round((float)__HAL_TIM_GET_AUTORELOAD(PWM->htim)*(duty/100));
	__HAL_TIM_SET_COMPARE(PWM->htim,PWM->Channel,pulseValue);
}
/*
 *
 */
void PWM_IC_Monitor(struct PWM_IC_signal* signal){
	float checkPeriod = (100/signal->frequency)*(8.05f);
	uint32_t msDelay;
	if(checkPeriod < 1){
		msDelay = 1;
	}
	else{
		msDelay = (int)ceil(checkPeriod);
	}
	if((HAL_GetTick() - signal->clock) > msDelay){
		if(!signal->readFlag){
			signal->duty = 0;
		}
		signal->readFlag = false;
	}
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
void PWM_IC_Init(struct PWM_IC_signal* signal,
		            TIM_HandleTypeDef *htim,
                    int frequency,
                    bool isChannel1)
{
    signal->duty = 0.0f;
    signal->readFlag = false;
    signal->frequency = frequency;
    signal->icVal = 0;
    signal->ch1 = isChannel1;
    signal->clock = HAL_GetTick();
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
 * @brief  Starts PWM generation on a specified timer channel and initializes PWM parameters.
 * @param  htim Pointer to the TIM HAL handle used for PWM generation.
 * @param  Channel Specifies the TIM channel to start PWM on.
 *                 This parameter can be one of the following values:
 *                 @arg TIM_CHANNEL_1
 *                 @arg TIM_CHANNEL_2
 *                 @arg TIM_CHANNEL_3
 *                 @arg TIM_CHANNEL_4
 * @param  duty Duty cycle in the range [0.0, 100.0].
 * @param  frequency PWM signal frequency.
 * @param  PWM Pointer to PWM_signal structure to initialize.
 * @note   While generally calling this function on an already running PWM signal
 *         is safe, it may cause a short glitch in the signal.
 */
void PWM_Out_Init(struct PWM_Out_signal *PWM, TIM_HandleTypeDef *htim, uint32_t Channel, float duty,int frequency)
{

	HAL_TIM_PWM_Start(htim, Channel);
	PWM->duty = duty;
	PWM->Channel = Channel;
	PWM->frequency = frequency;
	PWM->htim = htim;
	PWM_Out_setDuty(PWM, duty);
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
 * The result is stored in PWM_signal::duty as a percentage
 * value in the range 0–100.
 *
 * @param htim Pointer to the timer handle used for input capture
 * @param PWM Pointer to initialized PWM_signal structure
 */
void PWM_IC_update(struct PWM_IC_signal *PWM, TIM_HandleTypeDef *htim)
{
	PWM->readFlag = true;
    if (PWM->ch1)
    {
        PWM->icVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

        if (PWM->icVal != 0)
        {
            PWM->duty =
                ((float)HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) * 100.0f)
                / (float)PWM->icVal;
        }
    }
    else
    {
        PWM->icVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

        if (PWM->icVal != 0)
        {
            PWM->duty =
                ((float)HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) * 100.0f)
                / (float)PWM->icVal;
        }
    }
}
