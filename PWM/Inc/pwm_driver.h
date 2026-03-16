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
 * PWM signal
 */
struct PWM_signal {
    uint32_t frequency;
    volatile float width;
    volatile bool readFlag;
    volatile uint32_t icVal;
    bool ch1;
    TIM_IC_InitTypeDef sConfigIC;
};
/**
 * Set PWM signal up for Input Capture
 */
void PWM_readInit(struct PWM_signal* signal, int frequency, bool isChannel1,TIM_HandleTypeDef *htim);
/*
 * Set PWM signal up for being sent
 */
void PWM_generateInit(TIM_HandleTypeDef *htim, uint32_t channel);
/**
 * Computes PWM parameters, to be used within HAL_TIM_IC_CaptureCallback
 */
void PWM_update(TIM_HandleTypeDef *htim, struct PWM_signal *PWM);
/**
 * Set PWM signal width
 */
void PWM_setWidth(TIM_HandleTypeDef *htim, uint32_t Channel, float width);
#endif /* PWM_SIGNAL_H */
