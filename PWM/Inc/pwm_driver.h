/**
  * @file pwm_driver.h
  * @brief PWM signal driver for PERLA
  * @author AGH EKO-ENERGIA
  * @author Andrzej Gondek
  */

#ifndef PWM_SIGNAL_H
#define PWM_SIGNAL_H

#include "main.h"
#include <math.h>
#include <stdbool.h>

/**
 * PWM IC signal
 */
struct PWM_icSignal {
    uint32_t frequency;
    volatile float duty;
    volatile bool readFlag;
    volatile uint32_t icVal;
    bool ch1;
    TIM_IC_InitTypeDef sConfigIC;
};
/*
 * PWM signal
 */
struct PWM_signal{
	TIM_HandleTypeDef* htim;
	int frequency;
	uint32_t Channel;
	float duty;

};
/**
 * Set PWM signal up for Input Capture
 */
void PWM_readInit(struct PWM_icSignal* signal, int frequency, bool isChannel1,TIM_HandleTypeDef *htim);
/*
 * Set PWM signal up for being sent
 */
void PWM_generateInit(TIM_HandleTypeDef *htim, uint32_t Channel, float width,int frequency,struct PWM_signal *PWM);
/**
 * Computes PWM parameters, to be used within HAL_TIM_IC_CaptureCallback
 */
void PWM_update(TIM_HandleTypeDef *htim, struct PWM_icSignal *PWM);
/**
 * Set PWM signal duty
 */
void PWM_setDuty(struct PWM_signal *PWM);
#endif /* PWM_SIGNAL_H */
