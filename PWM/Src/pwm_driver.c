/**
  * @file pwm_driver.c
  * @brief PWM signal driver for PERLA
  * @author AGH EKO-ENERGIA
  * @author Andrzej Gondek
  */

#include "pwm_driver.h"
#include"main.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/**
 * @breif initiate PWM signal
 */
void PWM_initialize(struct PWM_signal* signal,int frequency, bool isChannel1, TIM_HandleTypeDef *htim ) {
	signal->PWM_width = 0.f;
	signal->readFlag = false;
	signal->frequency = frequency;
	signal->icVal = 0;
	signal->ch1 = isChannel1;
	signal->sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	signal->sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	signal->sConfigIC.ICPrescaler = TIM_ICPSC_DIV8;
	signal->sConfigIC.ICFilter = 0;
	if(isChannel1){
		HAL_TIM_IC_ConfigChannel(htim, &signal->sConfigIC, TIM_CHANNEL_1);
	}
	else{
		HAL_TIM_IC_ConfigChannel(htim, &signal->sConfigIC, TIM_CHANNEL_2);
	}

}
/**
 * @brief Read and compute PWM signal parameters
 * @param htim pointer to input capture timer
 * @param PWM pointer to a PWM_signal struct
 * @param isChannel1 bool whether combine channels are set to PWM input on channel one, false means that channels are set to PWM input on channel 2
 */
void PWM_update(TIM_HandleTypeDef *htim,struct PWM_signal *PWM, bool isChannel1)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1 && isChannel1 )
	{
		PWM->icVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

		if (PWM->icVal != 0)
		{
			PWM->PWM_width = (HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) *100)/PWM->icVal;

		}
	}
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 && !isChannel1 )
	{
		PWM->icVal = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

		if (PWM->icVal != 0)
		{
			PWM->PWM_width = (HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) *100)/PWM->icVal;

		}
	}
}
