#include "pwm_driver.h"
#include"main.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


void PWM_Initialize(PWM_Signal* signal,int frequency) {
	signal->PWM_Width = 0.f;
	signal->Read_Flag = false;
	signal->Frequency = frequency;
	signal->Capture_count = 0;
	signal->Widths[0] = 0.f;
	signal->Widths[1] = 0.f;
	signal->Widths[2] = 0.f;
	signal->Widths[3] = 0.f;
	signal->Widths[4] = 0.f;
}



void PWM_Update(TIM_HandleTypeDef *htim, PWM_Signal *PWM, uint32_t channel)
{
    if (PWM->Capture_count == 0)
    {
    	PWM->IC_Val1 = HAL_TIM_ReadCapturedValue(htim, channel);
        if (PWM->Read_Flag)
        {
        	PWM->Widths[4] = PWM->Widths[3];
            PWM->Widths[3] = PWM->Widths[2];
			PWM->Widths[2] = PWM->Widths[1];
		    PWM->Widths[1] = PWM->Widths[0];
            PWM->Widths[0] = (float)(PWM->IC_Val2) / ((float)PWM->IC_Val1);
        	PWM->PWM_Width = (PWM->Widths[0]+PWM->Widths[1]+PWM->Widths[2]+PWM->Widths[3]+PWM->Widths[4])/(float)(5);
        }
        __HAL_TIM_SET_COUNTER(htim, 0);
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_FALLING);
        PWM->Capture_count = 1;
    }
    else
    {
    	PWM->IC_Val2 = HAL_TIM_ReadCapturedValue(htim, channel);
        __HAL_TIM_SET_CAPTUREPOLARITY(htim, channel, TIM_INPUTCHANNELPOLARITY_RISING);
        PWM->Capture_count = 0;
    }

    PWM->Read_Flag = true;
}
